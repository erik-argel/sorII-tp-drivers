# Sistemas Operativos y Redes 2: Trabajo Práctico Device-Drivers

## Modulo Hola Mundo

## Introducción

En este trabajo, se solicita elaborar un kernel module para un char device el cual imprima en el kernel lo que le escribamos. Despues se solicita modificar lo que nos devuelve para que se visualice al reves caracter por caracter.

Para la primera parte donde imprime lo que le escribimos se definieron los siguientes archivos de la siguiente manera:

### Codigo char_device.c
```c

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h> 

static int device_open (struct inode *, struct file *) ;
static int device_release (struct inode *, struct file *) ;
static ssize_t device_read (struct file *, char *, size_t , loff_t *) ;
static ssize_t device_write (struct file *, const char *, size_t , loff_t *);


#define Nombre_Dispositivo "CharDevice-SORII"
#define Tamanio_Buffer 80

static int Major;
static int Dispositivo_Abierto = 0;

static char msj[Tamanio_Buffer];
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

    msg_Ptr = msj;
    return i;
}


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Argel Erik");
MODULE_DESCRIPTION("Un driver para el tp de SOR II");


```

### Codigo Makefile

```shell

obj-m := char_device.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
  
```

Para compilara el codigo de nuestro device se utilizara el comando:

```shell
make clean & make
```
![imagenMCM](imagenes/make_clean_chardv.png)

Resultado del comando anterior:

![imagenCDHW2](imagenes/result_make_clean.png)

Ahora para cargar el modulo al kernel se utilizara el comando:

```shell
insmod char_device.ko
```
![imagenIHW](imagenes/insmod_chardv.png)

Luego utilizando el comando:

```shell
dmesg
```

Se puede apreciar los mensajes del nucleo (kernel), y al final de estos los generado al cargar el modulo. En ellos indica el major number que le fue asignado y como debe hacer para comunicarse con él.

![imagenIHW](imagenes/result_insmod.png)

Se pasa a crear un archivo dev para comunicarse con el driver con el comando especificado en los mensajes anteriormente dados al cargar el modulo.

![imagenIHW](imagenes/mknod_chardev.png)

Se habilitaron todos los permisos para que no haya problemas despues.

![imagenIHW](imagenes/chmod_chardev.png)

Aca se puede ver dos ejemplos de cominucación con el char device utilizando los comandos echo y cat.

![imagenIHW](imagenes/echo_cat_chardev.png)

![imagenIHW](imagenes/echo_cat_2_chardev.png)

Utilizando el comando:

```shell
ls -la /dev
```

Se pueden ver todos los dispositivos, y entre ellos el char device recientemente creado.

![imagenIHW](imagenes/ls_la_chardev.png)

Por ultimo, para remover el modulo se utiliza el comando:

```shell
rmmod char_device.ko
```

![imagenIHW](imagenes/remove_chardev.png)

Y se puede ver con el comando:

```shell
dmesg
```

como se desregistra nuestro driver.

![imagenIHW](imagenes/result_remove_chardev.png)

Para la segunda parte donde imprime al reves lo que le escribimos, para facilitar su prueba se creo la carpeta 'charDev_reverse' en donde se encuentran el archivo 'charDev_reverse.c' y su Makefile. Estos son similares a el char_device original pero con las siguientes modificaciones:

### Codigo charDev_reverse.c
1. Se modifico el nombre para detectarlo mas facilmente.
```c
#define Nombre_Dispositivo "CharDeviceReverse-SORII"
```
2. Se agrego una funcion reverse la cual recibe un conjunto de caracteres y los devuelve de manera inversa.
```c
static char* reverse(char* input, size_t len) {
    int j=0;
    int k;
    for(k=len-2;k>=0; --k){
        msj_reverse[j++] = input[k];
    }
    msj_reverse[len-1] = input[len-1];
    return msj_reverse;
}
```
3. Se modifico la funcion device_write, la cual ahora llama a la funcion reverse antes de escribir el mensaje.
```c
static ssize_t device_write(struct file *filp, const char *buff, size_t len, loff_t * off){

   int i;
   #ifdef DEBUG
        printk(KERN_INFO "device_write(%p,%s,%d)",filp,buff,len);
   #endif
    for(i=0;i<len && i<Tamanio_Buffer;i++)get_user(msj[i],buff+i);

    msg_Ptr = reverse(msj,len);
    return i;
}
```

Codigo completo con las modificaciones:
```c
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
```

### Codigo Makefile

```shell

obj-m := charDev_reverse.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
  
```

Por ultimo para compilar y empezar a utilizar este char_device se utilizan los mismos comandos que con el original(teniendo en cuenta el cambio de nombre que se hizo). Aun asi deje todos juntos los comandos necesarios para su uso:
```shell
  make clean & make
  insmod charDev_reverse.ko
  sudo mknod /dev/CharDeviceReverse-SORII c 234 0
  sudo chmod 777 /dev/CharDeviceReverse-SORII
  echo "hola" > /dev/CharDeviceReverse-SORII
  cat /dev/CharDeviceReverse-SORII
  rmmod charDev_reverse.ko 
```

Por ultimo aca hay dos ejemplos de cominucación con el nuevo char device reverse utilizando los comandos echo y cat.


![imagenIHW](imagenes/chardevrevcat1.png)

![imagenIHW](imagenes/chardevrevcat2.png)
