#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h> 
#include <linux/ctype.h> 

/*   
*  Prototipos 
*/
int init_module(void);
void cleanup_module(void);
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

#define SUCCESS 0
#define DEVICE_NAME "chardev"   /* Nombre del dispositivo que va a aparecer en /proc/devices */
#define BUF_LEN 80              /* Maxima longitud del mensaje ingresado para el dispositivo */
#define CONST_N 1               /* Valor que utilizo para la encriptacion con cesar */
/*  
* Variables globales
*/
static int Major;               /* Major number asignado al driver */
static int Device_Open = 0;     /* Lo usamos para prevenir que se habran multiples accesos al dispositivo */
static char msg[BUF_LEN];       /* El mensaje que el dispositivo va a devolver cuando sea consultado */
static char *memoria_buffer;           /* Memoria del buffer */
static struct file_operations fops = {  
  .read = device_read,        
  .write = device_write,        
  .open = device_open,        
  .release = device_release
};

int init_module(void)
{ /* Constructor */
  Major = register_chrdev(0, DEVICE_NAME, &fops);        
  if (Major < 0) 
  {          
    printk(KERN_ALERT "Fallo al intentar registrar el dispositivo char %d\n", Major);          
    return Major;        
  }
  printk(KERN_INFO "Driver chardev registrado");
  printk(KERN_INFO "Tengo major number %d. Hablarle al driver ", Major);
  printk(KERN_INFO ", crear un dev_file con \n");
  printk(KERN_INFO "sudo rm /dev/%s\n", DEVICE_NAME);
  printk(KERN_INFO "sudo mknod /dev/%s c %d 0\n", DEVICE_NAME, Major);
  printk(KERN_INFO "sudo chmod 666 /dev/%s\n", DEVICE_NAME);
  printk(KERN_INFO "Probar varios minor numbers. Probar cat y echo\n");
  printk(KERN_INFO "Al device file.\n");
  printk(KERN_INFO "Eliminar el /dev y el modulo al terminar.\n");
  
  return 0;
}

void cleanup_module(void)
{ /* Destructor */
  unregister_chrdev(Major, DEVICE_NAME); //Desregistramos el dispositivo
  printk(KERN_INFO "Driver chardev desregistrado\n");
}
/*  
* LLamado cuando un usuario intenta abrir el dispositivo, "cat /dev/mycharfile" 
*/
static int device_open(struct inode *inode, struct file *file)
{                    
  #ifdef DEBUG
  printk(KERN_INFO "device_open(%p)\n", file);
  #endif

  if (Device_Open)                
    return -EBUSY;        
  Device_Open++;            

  memoria_buffer = msg;     
  return SUCCESS;
}
/*  
* LLamado cuando un usuario cierra el dispositivo
*/
static int device_release(struct inode *inode, struct file *file)
{          
  Device_Open--;          /* Desminuimos el valor, para determinar que esta listo para el proximo llamado */      
  return 0;
}
/*  
* LLamado cuando un usuario que abrio el dispositivo, intenta leerlo
*/
static ssize_t device_read(struct file *filp, char *buffer, size_t length, loff_t * offset)
{
  int bytes_read = 0;    // Numero de bytes escritos en el buffer
  if (*memoria_buffer == 0)     //si estamos en el final del mensaje retornamos 0           
    return 0;        
  
  while (length && *memoria_buffer) {        //ponemos la informacion en el buffer
    put_user(*(memoria_buffer++), buffer++);             
    length--;                
    bytes_read++;        
    // copy_to_user(to,from,bytes_read);      //mando mensajes al usuario (output) 
  }  
  printk(KERN_INFO "Dispositivo mostrando mensaje encriptado:\n");
  return bytes_read;  //La mayoria de las funciones de lectura, retornan la cantindad de bytes puestos en el buffer
 }

/*   
* Llamado cuando un usuario escribe al chardev: echo "hi" > /dev/chardev
*/
static ssize_t device_write(struct file *filp, const char *buff, size_t length, loff_t * off)
{        
  int i;
 // copy_from_user(to,from,bytes);  //traigo lo que puso el usuario (input)
  for (i = 0; i < length && i < BUF_LEN; i++){
    get_user(msg[i], buff + i);       //Guardo el mensaje en un array asi puedo operar en el
   // msg[i] = cesar(msg[i], CONST_N);  //Aplico cesar y modifico la posición en el array
  }

  msg[length]= '\0';
  memoria_buffer = msg;
  printk(KERN_INFO "Dispositivo encripto el mensaje:\n");    

  return length;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("UNGS");
MODULE_DESCRIPTION("Un primer driver");
