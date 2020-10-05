# Trabajo Practico Inicial de Sistemas Operativos y Redes II
## Device Drivers - Char Device

Primero tenemos que hacer andar el modulo, para eso, tenemos que seguir los siguientes pasos.

Primero ejecutamos el MakeFile.
$ make clean
$ make all

Utilizamos el siguiente comando, para instalar el modulo del kernel:
$sudo insmod miModulo.ko

Ahora para poder ver que se haya ejecutado el metodo init_module de miModulo, tenemos que usar el comando:
$d mesg

Gracias a este comando, podemos ver todo lo que se imprima en el modulo, por lo tanto, ahora se deberia de ver el siguiente mensaje en nuestra terminal:



Ypara removerlo usamos:
$rmmod miModulo.ko











Utilizamos el siguiente comando, para instalar el modulo del kernel:
$ sudo insmod chardev.ko
$ lsmod | grep chardev

Creamos el archivo asociado al driver:
$ sudo mknod /dev/chardev c 12 2
$ sudo chmod 666 /dev/chardev

Chequeamos que se haya creado 
$ ls /dev

Ingresamos el texto que sera encriptado:
$ sudo echo "Este mensaje será encriptado" >> /dev/chardev
$ sudo cat /dev/chardev

$dmesg


Y para removerlo usamos:
$ sudo rmmod chardev.ko && sudo rm /dev/chardev && make clean
