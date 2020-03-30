/**
 * Based on http://tldp.org/LDP/lkmpg/2.6/html/lkmpg.html#AEN40
 * 
 * This program will generate a random text flag when /proc/flag
 * is read.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/random.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

MODULE_AUTHOR("Omar Darwish");
MODULE_LICENSE("MIT");

#define PROC_NAME "flag"
#define FLAG_SIZE 1024

static unsigned long counter = 0;
static void *flag_start(struct seq_file *file, loff_t *pos) {
  if ( *pos == 0 ) {  
    counter = 0;
    return &counter;
  }
  else {
    *pos = 0;
    return NULL;
  }
}

static void *flag_next(struct seq_file *file, void *v, loff_t *pos) {
  (*pos)++;
  if (counter < FLAG_SIZE) {
    if (counter % 64 == 0) {
      seq_printf(file, "\n");
      (*pos)++;
    }

    return pos;
  }

  seq_printf(file, "\n");
  return NULL;
}

static void flag_stop(struct seq_file *file, void *v) { }

static int flag_show(struct seq_file *file, void *pos) {
  unsigned char num;
  get_random_bytes(&num, 1);
  num = num % 16;

  seq_printf(file, "%X", num);
  counter++;

  return 0;
}

static struct seq_operations seqops = {
  .start = flag_start,
  .next = flag_next,
  .stop = flag_stop,
  .show = flag_show
};

static int flag_open(struct inode *inode, struct file *file) {
  return seq_open(file, &seqops);
}

static const struct file_operations fops = {
  .owner = THIS_MODULE,
  .open = flag_open,
  .read = seq_read,
  .llseek = seq_lseek,
  .release = seq_release,
};

int init_module(){
  struct proc_dir_entry *entry;

  entry = proc_create(PROC_NAME, 0, NULL, &fops);
  printk("Loaded /proc/%s module\n", PROC_NAME);
  return 0;
}

void cleanup_module() {
  remove_proc_entry(PROC_NAME, NULL);
  printk("Unloaded /proc/%s module\n", PROC_NAME);
}
