import numpy as np
import random
import pandas as pd
from mpl_toolkits.mplot3d import Axes3D
import matplotlib.pyplot as plt


class MOPSO:
    def __init__(self, N, x_min, x_max, y_min, y_max, T, fw=500):
        self.N = N
        self.x_min = x_min
        self.x_max = x_max
        self.y_min = y_min
        self.y_max = y_max
        self.T = T
        self.pos = []
        self.fw = fw

    def set_objective_functions(self, func1, func2):
        self.objective_func_1 = func1
        self.objective_func_2 = func2

    def initialize_position_velocity(self):
        self.ps = [{"x": random.uniform(self.x_min, self.x_max),
                    "y": random.uniform(self.y_min, self.y_max)} for i in range(self.N)]
        self.vs = [{"x": 0.0, "y": 0.0} for i in range(self.N)]
        self.w1, self.w2 = 0.5, 0.5

    def set_personal_global_best(self):
        self.personal_best_positions = list(self.ps)
        self.personal_best_scores = [
            self.objective_func(p["x"], p["y"]) for p in self.ps]
        self.best_particle = np.argmin(self.personal_best_scores)
        self.global_best_position = self.personal_best_positions[self.best_particle]

    def update_particles(self, t):
        for n in range(self.N):
            # Get the position/velocity and parsonal best of each particle.
            x, y = self.ps[n]["x"], self.ps[n]["y"]
            vx, vy = self.vs[n]["x"], self.vs[n]["y"]
            p = self.personal_best_positions[n]

            # Update the particle position
            new_x, new_y = self.update_position(x, y, vx, vy)
            self.ps[n] = {"x": new_x, "y": new_y}
            # Update particle velocity
            new_vx, new_vy = self.update_velocity(
                new_x, new_y, vx, vy, p, self.global_best_position)
            self.vs[n] = {"x": new_vx, "y": new_vy}
            # Update PBests
            # Update function weights based on DWA
            self.w1, self.w2 = self.update_function_weight(t)
            score = self.objective_func(new_x, new_y)
        if t % 25 == 0:
            self.pos.append(self.ps)

            if score < self.personal_best_scores[n]:
                self.personal_best_scores[n] = score
                self.personal_best_positions[n] = {"x": new_x, "y": new_y}
        # Update Gbests
        self.best_particle = np.argmin(self.personal_best_scores)
        self.global_best_position = self.personal_best_positions[self.best_particle]

    # Update the weights dynamically with DWA algorithm.
    def update_function_weight(self, t):
        w1 = np.absolute(np.sin(2 * np.pi * t / self.fw))
        w2 = 1 - w1

        return w1, w2

    # Weighted sum of objective functions
    def objective_func(self, x, y):
        return self.w1 * self.objective_func_1(x, y) + self.w2 * self.objective_func_2(x, y)

    # Update particles' positions
    def update_position(self, x, y, vx, vy):
        new_x = x + vx
        new_y = y + vy
        return new_x, new_y

    # Update particles' velocities
    def update_velocity(self, x, y, vx, vy, p, g, w=0.5, ro_max=0.14):
        # Set the parameters randomly
        ro1 = random.uniform(0, ro_max)
        ro2 = random.uniform(0, ro_max)
        # Update particles' velocities
        new_vx = w * vx + ro1 * (p["x"] - x) + ro2 * (g["x"] - x)
        new_vy = w * vy + ro1 * (p["y"] - y) + ro2 * (g["y"] - y)
        return new_vx, new_vy

    # z = x^2 + y^2
    def objective_func_1_default(self, x, y):
        # z = x * x + y * y
        X, Y = np.meshgrid(x, y)
        z = X * X + Y * Y
        return z

    # z = sin(x) + cos(y)
    def objective_func_2_default(self, x, y):
        X, Y = np.meshgrid(x, y)
        z = np.sin(X) + np.cos(Y)
        return z


# Save the positions of particles for each 25 steps
def save_result_to_csv(pos, file_path):
    index = 0
    df = pd.DataFrame()
    for pos_t in pos:
        for ps in pos_t:
            df = df.append(
                {'index': index, 'x': ps['x'], 'y': ps['y']}, ignore_index=True)
        index += 1
    df.to_csv(file_path)


# Plot the particles' positions on 3D surfaces.
def visualize_result(file_path, x_min, x_max, y_min, y_max):
    x = np.arange(x_min, x_max, 0.25)
    y = np.arange(y_min, y_max, 0.25)
    X, Y = np.meshgrid(x, y)
    Z = X * X + Y * Y
    Z2 = np.sin(X) + np.cos(Y)

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
            z_2.append(np.sin(row['x']) + np.cos(row['y']))

    ax.scatter(x_1, y_1, z_1, c="r")
    ax.scatter(x_1, y_1, z_2, c="g")

    plt.show()


def main():
    mopso = MOPSO(N=100, x_min=-5.0, x_max=5.0,
                  y_min=-5.0, y_max=5.0, T=200, fw=500)
    mopso.initialize_position_velocity()

    # Define the functions, which are optimized by mopso.
    def square(x, y):
        X, Y = np.meshgrid(x, y)
        z = X * X + Y * Y
        return z

    def sin_cos(x, y):
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


if __name__ == '__main__':
    main()
