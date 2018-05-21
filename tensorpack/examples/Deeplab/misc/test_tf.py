import tensorflow as tf
import numpy as np

a = tf.placeholder(dtype=np.float32,shape=(3,3))
b = tf.placeholder(dtype=np.float32,shape=(3,3))

a_value = np.array([[1,2,3],[4,5,6],[7,8,9]],dtype=np.float32)

result = tf.to_float(tf.greater(a,4))*b

with tf.Session() as sess:
    r = sess.run([result],feed_dict={a:a_value,b:a_value})
    print r


