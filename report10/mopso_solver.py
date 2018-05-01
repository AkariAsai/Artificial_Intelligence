from mopso import MOPSO
# This is for the vizualization
from mpl_toolkits.mplot3d import Axes3D
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

def save_result_to_csv(pos, file_path):
    index = 0
    df = pd.DataFrame()
    for pos_t in pos:
        for ps in pos_t:
            df = df.append({'index': index, 'x': ps['x'] ,'y':ps['y']}, ignore_index=True)
        index += 1
    df.to_csv(file_path)
    
def visualize_result(file_path, x_min, x_max, y_min, y_max):
    x = np.arange(x_min, x_max, 0.25)
    y = np.arange(y_min, y_max, 0.25)
    X, Y = np.meshgrid(x, y)
    Z = X*X + Y * Y
    Z2= np.sin(X)+ np.cos(Y)

    fig = plt.figure()
    ax = Axes3D(fig)
    ax.plot_wireframe(X, Y, Z, rstride=1, cstride=1, color="blue", alpha=0.3)
    ax.plot_wireframe(X, Y, Z2, rstride=1, cstride=1, color="blue", alpha=0.3)

    df = pd.read_csv(file_path)
    df_1 = df.loc[df.index == 0.0]
    print(df_1)

    x_1 = []
    y_1 = []
    z_1 = []
    z_2 = []
    for index, row in df.iterrows():
        if row["index"] == 7.0:
            x_1.append(row['x'])
            y_1.append(row['y'])

            z_1.append(row['x'] ** 2 + row['y'] ** 2)
            z_2.append(np.sin(row['x'])+ np.cos(row['y']))

    ax.scatter(x_1, y_1, z_1, c="r")
    ax.scatter(x_1, y_1, z_2, c="g")

    plt.show()
    
def main():
    mopso = MOPSO(N=100, x_min=-5.0, x_max=5.0, y_min= -5.0, y_max=5.0, T=200, fw=500)
    mopso.initialize_position_velocity()
    
    # Define the functions, which are optimized by pso.
    def square(x, y):
        X, Y = np.meshgrid(x, y)
        z = X*X+ Y*Y
        return z
    
    def sin_cos(x,y):
        X, Y = np.meshgrid(x, y)
        z = np.sin(X) + np.cos(Y)
        return z
    
    # Set the objectvie functions
    mopso.set_objective_functions(square, sin_cos)
    # Set the initial global/personal best scores
    mopso.set_personal_global_best()
    
    for t in range(mopso.T):
        mopso.update_particles(t)
        

    print(mopso.global_best_position)
    print(min(mopso.personal_best_scores))
    
    save_result_to_csv(mopso.pos, "ps.csv")
    visualize_result("ps.csv", -5.0, 5.0, -5.0, 5.0)
    

if __name__ == "__main__":
    main()