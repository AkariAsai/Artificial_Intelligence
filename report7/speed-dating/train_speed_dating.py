# coding: utf-8
import sys
import numpy as np
import matplotlib.pyplot as plt
from speed_dating import load_speed_dating
from two_layer_net import TwoLayerNet
import time

(x_train, t_train), (x_test, t_test) = load_speed_dating(
    normalize=True, one_hot_label=True, feature_selection=False)

input_size = len(x_train[0])
network = TwoLayerNet(input_size=input_size, hidden_size=50, output_size=2)

iters_num = 50000
train_size = x_train.shape[0]
batch_size = 100
learning_rate = 1

train_loss_list = []
train_acc_list = []
test_acc_list = []

iter_per_epoch = max(train_size / batch_size, 1)

start = time.time()
for i in range(iters_num):
    batch_mask = np.random.choice(train_size, batch_size)
    x_batch = x_train[batch_mask]
    t_batch = t_train[batch_mask]

    # 勾配の計算
    grad = network.gradient(x_batch, t_batch)

    # パラメータの更新
    for key in ('W1', 'b1', 'W2', 'b2'):
        network.params[key] -= learning_rate * grad[key]

    loss = network.loss(x_batch, t_batch)

    if i % 100 == 0:
        train_loss_list.append(loss)
        train_acc = network.accuracy(x_train, t_train)
        test_acc = network.accuracy(x_test, t_test)
        train_acc_list.append(train_acc)
        test_acc_list.append(test_acc)
        print(str(i) + " : train acc, test acc | " +
              str(train_acc) + ", " + str(test_acc) + ", loss : " + str(loss))

# training lossのグラフを描画
elapsed_time = time.time() - start
print ("elapsed_time:{0}".format(elapsed_time) + "[sec]")
markers = {'train': 'o', 'test': 's'}
x = np.arange(len(train_acc_list))
plt.plot(x, train_loss_list, label="train_loss_list")
plt.xlabel("epochs")
plt.ylabel("loss")
plt.ylim(0, 1.0)
plt.legend(loc='lower right')
plt.show()


plt.plot(x, train_acc_list, label="trainig_accuracy")
plt.plot(x, test_acc_list, label="test_accuracy")
plt.xlabel("epochs")
plt.ylabel("accuracy")
plt.ylim(0, 1.0)
plt.legend(loc='lower right')
plt.show()
