# Sistemas Operativos y Redes 2: Trabajo Práctico Device-Drivers

## Modulo Hola Mundo

## Introducción

En este trabajo, se crea un modulo de ejemplo basico se lo carga y descarga, y se toman algunas capturas de su funcionamiento.

### Codigo miModulo.c
```c

#include <linux/module.h>
#include <linux/kernel.h>

int init_module ( void )
{ /* Constructor */
printk ( KERN_INFO "Argel: Driver registrado \n") ;
return 0;
}
void cleanup_module ( void )
{/* Destructor */
printk ( KERN_INFO "Argel: Driver desregistrado \n") ;
}
MODULE_LICENSE ("GPL") ;
MODULE_AUTHOR ("Argel Erik") ;
MODULE_DESCRIPTION ("Un primer driver") ;


```
### Codigo Makefile

```shell

obj-m := miModulo.o

all:
		make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
		make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean


```
Carpeta hello_world:

![imagenCDHW](imagenes/carpeta_hello_world.png)

Para compilara el codigo de nuestro device se utilizara el comando:

```shell
make clean & make
```
![imagenMCM](imagenes/clean_make.png)

Resultado del comando anterior:

![imagenCDHW2](imagenes/result_clean_make.png)

Ahora para cargar el modulo al kernel se utilizara el comando:

```shell
insmod miModulo.ko
```
![imagenIHW](imagenes/insmod_miModulo.png)

Luego utilizando el comando:

```shell
dmesg
```
Se puede apreciar los mensajes del nucleo (kernel), y al final de estos los generado al cargar el modulo. En este caso solo el mensaje de que el dirver se a registrado.

![imagenIHW](imagenes/result_dmesg.png)

Luego utilizando el comando:

```shell
modinfo miModulo.ko
```
Se puede ver informacion sobre el modulo.

![imagenIHW](imagenes/modinfo_miModulo.png)

Por ultimo, para remover el modulo se utiliza el comando:

```shell
rmmod miModulo.ko
```
![imagenIHW](imagenes/rmmod_miModulo.png)

Y se puede ver con el comando:

```shell
dmesg
```

como se desregistra nuestro driver.

![imagenIHW](imagenes/result_dmesg_remove.png)
