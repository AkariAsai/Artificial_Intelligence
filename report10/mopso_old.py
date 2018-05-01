import numpy as np
import random
import pandas as pd

# This is weight change frequency for DWA
fw = 500

# z = x^2 + y^2
def objective_func_1(x, y):
    # z = x * x + y * y
    X, Y = np.meshgrid(x, y)
    z = X*X+ Y*Y
    return z

# z = sin(x) + cos(y)
def objective_func_2(x,y):
    X, Y = np.meshgrid(x, y)
    z = np.sin(X)+ np.cos(Y)
    return z

# Update the weights dynamically with DWA algorithm.
def update_function_weight(w1, w2, t):
    w1 = np.absolute(np.sin(2 * np.pi * t / fw))
    w2 = 1 - w1

    return w1, w2

# Weighted sum of objective functions
def objective_func(x, y, w1, w2):
    return w1 * objective_func_1(x,y) + w2 * objective_func_2(x,y)

# Update particles' positions
def update_position(x, y, vx, vy):
    new_x = x + vx
    new_y = y + vy
    return new_x, new_y

# Update particles' velocities
def update_velocity(x, y, vx, vy, p, g, w=0.5, ro_max=0.14):
    # Set the parameters randomly
    ro1 = random.uniform(0, ro_max)
    ro2 = random.uniform(0, ro_max)
    # Update particles' velocities
    new_vx = w * vx + ro1 * (p["x"] - x) + ro2 * (g["x"] - x)
    new_vy = w * vy + ro1 * (p["y"] - y) + ro2 * (g["y"] - y)
    return new_vx, new_vy


def main():
    pos = []
    # The number of particles
    N = 100
    x_min, x_max = -5, 5
    y_min, y_max = -5, 5

    # Initialize the positions and velocities of each particle, and weights.
    ps = [{"x": random.uniform(x_min, x_max),
        "y": random.uniform(y_min, y_max)} for i in range(N)]
    vs = [{"x": 0.0, "y": 0.0} for i in range(N)]
    w1, w2 = 0.5, 0.5

    # Set the parsonal bests and global bests.
    personal_best_positions = list(ps)
    personal_best_scores = [objective_func(p["x"], p["y"], w1, w2) for p in ps]
    best_particle = np.argmin(personal_best_scores)
    global_best_position = personal_best_positions[best_particle]

    T = 200
    for t in range(T):
        for n in range(N):
            # Get the position/velocity and parsonal best of each particle.
            x, y = ps[n]["x"], ps[n]["y"]
            vx, vy = vs[n]["x"], vs[n]["y"]
            p = personal_best_positions[n]

            # Update the particle position
            new_x, new_y = update_position(x, y, vx, vy)
            ps[n] = {"x": new_x, "y": new_y}
            # Update particle velocity
            new_vx, new_vy = update_velocity(
                new_x, new_y, vx, vy, p, global_best_position)
            vs[n] = {"x": new_vx, "y": new_vy}
            # Update PBests
            # Update function weights based on DWA
            w1, w2 = update_function_weight(w1, w2, t)
            score = objective_func(new_x, new_y, w1, w2)

        # Save the positions to visualize
        if t % 25 == 0:
            pos.append(ps)

            if score < personal_best_scores[n]:
                personal_best_scores[n] = score
                personal_best_positions[n] = {"x": new_x, "y": new_y}
        # Update Gbests
        best_particle = np.argmin(personal_best_scores)
        global_best_position = personal_best_positions[best_particle]

    print(global_best_position)
    print(min(personal_best_scores))

    index  = 0
    df = pd.DataFrame()
    for pos_t in pos:
        for ps in pos_t:
            df = df.append({'index': index, 'x': ps['x'] ,'y':ps['y']}, ignore_index=True)
        index += 1
    df.to_csv("ps.csv")

if __name__ == '__main__':
    main()
