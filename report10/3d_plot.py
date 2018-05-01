from mpl_toolkits.mplot3d import Axes3D
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

x = np.arange(-5, 5, 0.25)
y = np.arange(-5, 5, 0.25)
X, Y = np.meshgrid(x, y)
Z = X*X + Y * Y
Z2= np.sin(X)+ np.cos(Y)

fig = plt.figure()
ax = Axes3D(fig)
ax.plot_wireframe(X, Y, Z, rstride=1, cstride=1, color="blue", alpha=0.3)
ax.plot_wireframe(X, Y, Z2, rstride=1, cstride=1, color="blue", alpha=0.3)

df = pd.read_csv("ps.csv")
df_1 = df.loc[df.index == 0.0]
print(df_1)

x_1 = []
y_1 = []
z_1 = []
z_2 = []
for index, row in df.iterrows():
    # print(row)
    if row["index"] == 7.0:
        x_1.append(row['x'])
        y_1.append(row['y'])

        z_1.append(row['x'] ** 2 + row['y'] ** 2)
        z_2.append(np.sin(row['x'])+ np.cos(row['y']))

ax.scatter(x_1, y_1, z_1, c="r")
ax.scatter(x_1, y_1, z_2, c="g")

plt.show()
