#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h> 

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

/*  
* Variables globales
*/
static int Major;               /* Major number asignado al driver */
static int Device_Open = 0;     /* Lo usamos para prevenir que se habran multiples accesos al dispositivo */
static char msg[BUF_LEN];       /* El mensaje que el dispositivo va a devolver cuando sea consultado */
static char *msg_Ptr;           /* Memoria del buffer */
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
  static int counter = 0;        
  if (Device_Open)                
    return -EBUSY;        
  Device_Open++;             
  msg_Ptr = msg;        
  try_module_get(THIS_MODULE);      
  return SUCCESS;
}
/*  
* LLamado cuando un usuario cierra el dispositivo
*/
static int device_release(struct inode *inode, struct file *file)
{          
  Device_Open--;          /* Desminuimos el valor, para determinar que esta listo para el proximo llamado */        
  module_put(THIS_MODULE);         
  return 0;
}
/*  
* LLamado cuando un usuario que abrio el dispositivo, intenta leerlo
*/
static ssize_t device_read(struct file *filp, char *buffer, size_t length, loff_t * offset)
{
  printk(KERN_ALERT "EMPEZO A LEER\n");        
  int bytes_read = 0;    // Numero de bytes escritos en el buffer
  if (*msg_Ptr == 0)     //si estamos en el final del mensaje retornamos 0           
    return 0;        
  
  while (length && *msg_Ptr) {        //ponemos la informacion en el buffer
    put_user(*(msg_Ptr++), buffer++);             
    length--;                
    bytes_read++;        
  }
 // copy_to_user(to,from,bytes_read);      //mando mensajes al usuario (output)   
  printk(KERN_ALERT "Dispositivo mostrando mensaje encriptado:\n");
  return bytes_read;  //La mayoria de las funciones de lectura, retornan la cantindad de bytes puestos en el buffer
 }

/*   
* Llamado cuando un usuario escribe al chardev: echo "hi" > /dev/chardev
*/
static ssize_t device_write(struct file *filp, const char *buff, size_t len, loff_t * off)
{        
  int i;
 // copy_from_user(to,from,bytes);  //traigo lo que puso el usuario (input)
  for (i = 0; i < len && i < BUF_LEN; i++)
    get_user(msg[i], buff + i);

  msg[len]= '\0';
  msg_Ptr = msg;
  printk(KERN_ALERT "Dispositivo encriptando:\n");    

  return -EINVAL;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("UNGS");
MODULE_DESCRIPTION("Un primer driver");
