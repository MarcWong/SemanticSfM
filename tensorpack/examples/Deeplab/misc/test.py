# Author: Tao Hu <taohu620@gmail.com>
import tensorflow as tf
import numpy as np
from tensorflow.python import debug as tf_debug

def my_softmax_cross_entropy_with_ignore_label(logits, label, class_num, mask):
        raw_prediction = tf.reshape(logits, [-1, class_num],name="raw_prediction")
        label = tf.reshape(label, [-1, class_num],name="label")

        mask = tf.reshape(mask,[-1],name="mask")
        indices = tf.squeeze(tf.where(tf.equal(mask, 1)), axis=1,name="indices_to_get") # maybe buggy

        ignore_indices = tf.squeeze(tf.where(tf.equal(mask, 0)), axis=1, name="indices_to_get_ignore")
        ignore_gt = tf.gather(label, ignore_indices)

        #with tf.control_dependencies([tf.assert_equal(tf.reduce_sum(ignore_gt),0)]):
        gt = tf.gather(label, indices,name="gt")
        prediction = tf.gather(raw_prediction, indices,name="prediction")
            # Pixel-wise softmax loss.
        loss = tf.nn.softmax_cross_entropy_with_logits(logits=prediction, labels=gt,name="loss")
        return loss


logits = tf.placeholder(dtype=tf.float32, shape= (2,3),name="logits")
label = tf.placeholder(dtype=tf.float32, shape = (2,3),name="label")
class_num = 3
mask = tf.placeholder(dtype=tf.float32, shape= (2,),name="mask")
loss = my_softmax_cross_entropy_with_ignore_label(logits, label, class_num, mask)
sess = tf.Session()
#sess = tf_debug.LocalCLIDebugWrapperSession(sess)

logits_value = np.array([[5,5,5],[0.5,0.5,0.5]])
label_value = np.array([[0,0.5,0.5],[1,0,0]])
mask_value = np.array([1,1])

result = sess.run([loss],feed_dict={logits:logits_value,label:label_value,mask:mask_value})

result = result[0]

print result