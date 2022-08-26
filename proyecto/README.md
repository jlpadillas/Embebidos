# **Proyecto Padilla**

1. Al arrancar el programa, el brazo se colocará en una posición inicial, independientemente de cómo se
encuentre en ese momento. A este estado, lo llamaremos "calibración"; y las luces parpadearán de color
amarillo durante todo el rato que esta fase dure.

2. La siguiente fase en la que se entrará será la de "espera", donde las luces se iluminarán de color verde a la
espera de que el usuario pulse algún botón de la botonera. El usuario podrá pulsar estos botones para mover
el brazo; y, dependiendo del botón que se pulse, el brazo realizará alguno de los siguientes movimientos:
    - Si se pulsa el botón central, el agarre se abrirá/cerrará.
    - Si se pulsa el botón de arriba, el brazo subirá.
    - Si se pulsa el botón de abajo, el brazo bajará.
    - Si se pulsa el botón derecho, el brazo se moverá a su derecha.
    - Si se pulsa el botón izquierdo, el brazo se moverá a su izquierda.

    Los movimientos serán permitidos siempre y cuando no se haya alcanzado el límite físico del brazo. Para ello se
    usarán diversos sensores que se han añadido al brazo: un sensor de pulsación para limitar el movimiento hacia
    la derecha; y, un sensor de color, que se usará para detectar si el brazo está en su posición más alta posible. En
    cualquiera de estos dos casos, el programa se bloqueará durante un breve lapso de tiempo, no aceptando más
    pulsaciones de los botones y parpadearán las luces de color rojo, así como se mostrará una “X” gigante en la
    pantalla que tiene como finalidad indicar que el brazo no puede moverse en la dirección que se ha pulsado.

3. Cuando el usuario pulse uno de los anteriores botones, el robot pasará a un estado de "movimiento", donde las
luces parpadearán de color verde y el brazo se moverá hasta el punto en el que desee el usuario.

4. Al terminar de realizar todo los movimientos deseados, se puede pulsar el botón de atrás para que el programa
finalice. A este estado lo llamamos “de parada”, donde el programa mostrará por pantalla un mensaje de
despedida, así como hará que las luces de color rojo parpadeen unos instantes antes de finalizar su ejecución.
