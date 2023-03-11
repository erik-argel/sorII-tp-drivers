#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h> 

static int device_open (struct inode *, struct file *) ;
static int device_release (struct inode *, struct file *) ;
static ssize_t device_read (struct file *, char *, size_t , loff_t *) ;
static ssize_t device_write (struct file *, const char *, size_t , loff_t *);
static char* reverse(char* input, size_t len);


#define Nombre_Dispositivo "CharDeviceReverse-SORII"
#define Tamanio_Buffer 80

static int Major;
static int Dispositivo_Abierto = 0;

static char msj[Tamanio_Buffer];
static char msj_reverse[Tamanio_Buffer];
static char *msg_Ptr;

static struct file_operations fops = {
 .read = device_read,
 .write = device_write,
 .open = device_open,
 .release = device_release
};

int init_module(void){ 
    /* Constructor */
    Major = register_chrdev(0, Nombre_Dispositivo, &fops);
    if (Major < 0) {
        printk(KERN_ALERT "No se pudo registrar el dispositivo char con %d\n", Major);
        return Major;
    }
    printk(KERN_INFO "Device: Driver %s registrado \n",Nombre_Dispositivo);
    printk(KERN_INFO "Device: Se le a asignado el número %d.\n", Major);
    printk(KERN_INFO "Device: Para comunicarse con el driver, cree un archivo dev con: 'mknod /dev/%s c %d 0'.\n",Nombre_Dispositivo,Major);
    printk(KERN_INFO "Device: Prueba varios números menores. Prueba hacer un cat y echo en el archivo del dispositivo.\n");
    printk(KERN_INFO "Device: Elimine el archivo del dispositivo y el modulo cuando haya terminado.\n");
    return 0;
}

void cleanup_module(void){ 
    /* Destructor */
    unregister_chrdev(Major, Nombre_Dispositivo);
    printk ( KERN_INFO "Device: Driver desregistrado \n");
}

static int device_open(struct inode *inode, struct file *file){
    if (Dispositivo_Abierto) return -EBUSY;
    Dispositivo_Abierto++;
    try_module_get(THIS_MODULE);
    return 0;
}

static int device_release(struct inode *inode, struct file *file){
    Dispositivo_Abierto--; 
    module_put(THIS_MODULE);
    return 0;
}

static ssize_t device_read(struct file *filp, char *buffer, size_t length, loff_t * offset){
    int bytes_read = 0;
    if (*msg_Ptr == 0)return 0;
    while (length && *msg_Ptr) {
        put_user(*(msg_Ptr++), buffer++);
        length--;
        bytes_read++;
    }
    return bytes_read;

}

static ssize_t device_write(struct file *filp, const char *buff, size_t len, loff_t * off){

   int i;
   #ifdef DEBUG
        printk(KERN_INFO "device_write(%p,%s,%d)",filp,buff,len);
   #endif
    for(i=0;i<len && i<Tamanio_Buffer;i++)get_user(msj[i],buff+i);

    msg_Ptr = reverse(msj,len);
    return i;
}

static char* reverse(char* input, size_t len) {
    int j=0;
    int k;
    for(k=len-2;k>=0; --k){
        msj_reverse[j++] = input[k];
    }
    msj_reverse[len-1] = input[len-1];
    return msj_reverse;
}


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Argel Erik");
MODULE_DESCRIPTION("Un driver para el tp de SOR II");
