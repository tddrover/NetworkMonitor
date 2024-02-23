#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/keyboard.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/ip.h>
#include <linux/skbuff.h>
#include <linux/netfilter.h>
#include <linux/inet.h>
#include <linux/socket.h>
#include <linux/string.h>

#define PROC_FILE_NAME "ip_tree"

struct rb_root root = RB_ROOT;

struct ip_count_node{
	struct rb_node node;
	void * ip;
	unsigned int ip_count;
};
struct nf_hook_ops demo_hook;

ssize_t read_simple(struct file *filp,char *buf,size_t count,loff_t *offp ) 
{
	struct ip_count_node *pos, *n;
	size_t sofar = 0;
	if(*offp)
		return 0;
	rbtree_postorder_for_each_entry_safe(pos, n, &root, node){
		sofar += snprintf(buf + sofar, count - sofar, "IP %pI4 : %u\n",pos->ip, pos->ip_count);
	
	}
	*offp = sofar;
	return sofar;
}

const struct proc_ops proc_fops = {
	.proc_read = read_simple, 
};

unsigned int hook_function(void *priv, struct sk_buff *skb, const struct nf_hook_state *state){
	struct iphdr *ip_header=(struct iphdr *)skb_network_header(skb);
	struct rb_node **new = &(root.rb_node), *parent = NULL;
	while(*new){
		struct ip_count_node *this=container_of(*new, struct ip_count_node, node);
		parent = *new;
		if(this->ip == &(ip_header->saddr)){    
			this->ip_count++;
			printk("Added to %pI4:%u\n", this->ip, this->ip_count);
			return 0;
	} else if((struct in_addr *) this->ip < &(ip_header->saddr))
		new = &((*new)->rb_right);
	else
		new = &((*new)->rb_left);
	}
	{
		struct ip_count_node *new_node = kmalloc(sizeof(struct ip_count_node), 1);
		new_node->ip=&(ip_header->saddr);    
		new_node->ip_count = 1;
		rb_link_node(&new_node->node, parent, new);
		rb_insert_color(&new_node->node, &root);
		printk("Added new node %pI4:%u\n",new_node->ip,new_node->ip_count);
	}	
	return NF_ACCEPT;
}
int init (void) {
	demo_hook.hook = hook_function;
	demo_hook.hooknum = NF_INET_LOCAL_IN;
	demo_hook.pf = AF_INET;
	nf_register_net_hook(&init_net, &demo_hook);

	proc_create(PROC_FILE_NAME,0,NULL,&proc_fops);
	return 0;
}

void cleanup(void) {
	struct ip_count_node *pos, *n;

	nf_unregister_net_hook(&init_net,&demo_hook);

	remove_proc_entry(PROC_FILE_NAME,NULL);

	printk("Beginning Tree Deallocation\n");
	rbtree_postorder_for_each_entry_safe(pos, n, &root, node){
		printk("IP %pI4 : %u\n", pos->ip, pos->ip_count);
		kfree(pos);
	}
}

MODULE_LICENSE("GPL"); 
module_init(init);
module_exit(cleanup);

