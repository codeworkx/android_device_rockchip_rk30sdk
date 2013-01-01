#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/poll.h>
#include <sys/inotify.h>
#include <linux/input.h>
#include <errno.h>
#include "minui.h"
#include "cutils/log.h"

#define MAX_DEVICES 16
#define BITS_PER_LONG (sizeof(unsigned long) * 8)
#define BITS_TO_LONGS(x) (((x) + BITS_PER_LONG - 1) / BITS_PER_LONG)

#define test_bit(bit, array) \
    ((array)[(bit)/BITS_PER_LONG] & (1 << ((bit) % BITS_PER_LONG)))
static struct pollfd ev_fds[MAX_DEVICES];
static char ev_names[MAX_DEVICES][255];
struct fd_info {
    ev_callback cb;
    void *data;
	struct fd_info *next;
};
static struct fd_info fd_callbacks;
static int notify_fd = 0;
static int notify_wd = 0;
static int ev_inited = 0;
static unsigned int ev_count = 0;
static void minui_log(const char * format, ...)
{
/*
	FILE *file;
	file = fopen("/tmp/mylog","a+");
	va_list args;
  	va_start (args, format);
  	vfprintf (file, format, args);
  	va_end (args);
	fprintf(file,"\n");
	fclose(file);
*/
	return;
}
static void __ev_add_fd(int fd,char *name){
	unsigned long ev_bits[BITS_TO_LONGS(EV_MAX)];
	if(fd < 0) return;
	if(ev_count >= MAX_DEVICES){
		close(fd);
		return;
	}
	minui_log("adding %s to read.",name);
	if (ioctl(fd, EVIOCGBIT(0, sizeof(ev_bits)), ev_bits) < 0) {
		close(fd);
        return;
	}
	minui_log("adding %s testing bit.",name);
    if (!test_bit(EV_KEY, ev_bits) && !test_bit(EV_REL, ev_bits) && !test_bit(EV_ABS, ev_bits)) {
    	close(fd);
        return;
    }
    minui_log("adding %s tested",name);
	ev_fds[ev_count].fd = fd;
    ev_fds[ev_count].events = POLLIN;
    strncpy(ev_names[ev_count],name,255);
    ev_count++;
}
static int init()
{
	DIR *dir;
    struct dirent *de;
    int fd;
	int i;
    dir = opendir("/dev/input");
    if(dir != 0) {
		ev_count = 0;
		notify_fd = inotify_init();
		if(notify_fd == -1){
			minui_log("inotify_init error,[%d]%s",errno,strerror(errno));
			return 1;
		}
	    notify_wd = inotify_add_watch(notify_fd, "/dev/input", IN_DELETE | IN_CREATE);
		if(notify_wd == -1){
			minui_log("inotify_add_watch error,[%d]%s",errno,strerror(errno));
			return 1;
		}
		ev_fds[0].fd = notify_fd;
		ev_fds[0].events = POLLIN;
		ev_count++;
		while(de=readdir(dir)){
            if(strncmp(de->d_name,"event",5)) continue;
            fd = openat(dirfd(dir), de->d_name, O_RDONLY);
			__ev_add_fd(fd,de->d_name);
			if(ev_count == MAX_DEVICES) break;
        }
		ev_inited = 1;
		return 0;
    }
	return 1;
}
int ev_init(ev_callback input_cb, void *data){
	if(!ev_inited) init();
	struct fd_info *cb = malloc(sizeof(struct fd_info));
	if(!cb) return -1;
	memset(cb,0,sizeof(struct fd_info));
	cb->cb = input_cb;
	cb->data = data;
	
	struct fd_info *tmp_fd = &fd_callbacks;
	while(tmp_fd->next)
		tmp_fd = tmp_fd->next;
	tmp_fd->next = cb;
	return 0;
}
int ev_add_fd(int fd, ev_callback cb, void *data)
{
	minui_log("ev_add_fd");
	return 0;
}
void ev_exit(void)
{
}
int ev_wait(int timeout){
	int r;
    r = poll(ev_fds, ev_count, timeout);
    if (r <= 0)
        return -1;
    return 0;
}
void ev_dispatch(void){
	unsigned n,i;	
	if(ev_fds[0].revents & ev_fds[0].events){
		unsigned char buf[1024] = {0};
		int len,index;
		struct inotify_event *event = NULL;
		len = read(ev_fds[0].fd, &buf, sizeof(buf));
		minui_log("read [%d] byes from ev_fds[0].fd",len);
		index = 0;
		while (index < len){
			event = (struct inotify_event *)(buf + index);
			minui_log("event->mask=%d",event->mask);
			if(IN_CREATE & event->mask){
         		minui_log("event->name: %s", event->name);
				if(!strncmp(event->name,"event",5)){
					char sock_name[255];
					snprintf(sock_name,255,"/dev/input/%s",event->name);
					int fd = open(sock_name,O_RDONLY);
					if(fd == -1){
						minui_log("can not open event->name[%s]",event->name);
					}
					else{
						__ev_add_fd(fd,event->name);
					}
					
				}
			}
			else if(IN_DELETE & event->mask){
				minui_log("%s deleted", event->name);
				for(n = ev_count -1 ; n > 0;n--){
					if(!strncmp(event->name,ev_names[n],255)){
						minui_log("found it. removing....");
						for(i = n;i < ev_count - 1; i++){
							strncpy(ev_names[i],ev_names[i+1],255);
						}
						memset(ev_names[ev_count-1],0,255);
						close(ev_fds[n].fd);
						for(i = n;i < ev_count - 1; i++){
							memcpy(&(ev_fds[i]),&(ev_fds[i+1]),sizeof(struct pollfd));
						}
						memset(&(ev_fds[ev_count-1]),0,sizeof(struct pollfd));
						--ev_count;
					}
					//we dont break here. just in case we have duplicate names.
				}
			}
			else{
				minui_log("event->name[%s],event->mask[%d]",event->name, event->mask);
			}
			index += sizeof(struct inotify_event) + event->len;
		}			
	}
    for (n = 1; n < ev_count; n++) {
        if (ev_fds[n].revents & ev_fds[n].events){
			minui_log("ev_fds[%d] has event.",n);
			struct fd_info *tmp_fd = fd_callbacks.next;
			while(tmp_fd){
				tmp_fd->cb(ev_fds[n].fd, ev_fds[n].revents, tmp_fd->data);
				tmp_fd = tmp_fd->next;
			}
		}
    }

}
int ev_get_input(int fd, short revents, struct input_event *ev)
{
    int r;

    if (revents & POLLIN) {
        r = read(fd, ev, sizeof(*ev));
        if (r == sizeof(*ev))
            return 0;
    }
    return -1;
}

int ev_sync_key_state(ev_set_key_callback set_key_cb, void *data)
{
    unsigned long key_bits[BITS_TO_LONGS(KEY_MAX)];
    unsigned long ev_bits[BITS_TO_LONGS(EV_MAX)];
    unsigned i;
    int ret;

    for (i = 1; i < ev_count; i++) {
        int code;

        memset(key_bits, 0, sizeof(key_bits));
        memset(ev_bits, 0, sizeof(ev_bits));

        ret = ioctl(ev_fds[i].fd, EVIOCGBIT(0, sizeof(ev_bits)), ev_bits);
        if (ret < 0 || !test_bit(EV_KEY, ev_bits))
            continue;

        ret = ioctl(ev_fds[i].fd, EVIOCGKEY(sizeof(key_bits)), key_bits);
        if (ret < 0)
            continue;

        for (code = 0; code <= KEY_MAX; code++) {
            if (test_bit(code, key_bits))
                set_key_cb(code, 1, data);
        }
    }

    return 0;
}


