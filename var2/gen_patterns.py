import os

ROWS, COLS = 25, 80

def save_pattern(filename, live_coords):
    grid = [[0] * COLS for _ in range(ROWS)]
    for r, c in live_coords:
        if 0 <= r < ROWS and 0 <= c < COLS:
            grid[r][c] = 1
    os.makedirs("patterns", exist_ok=True)
    with open(f"patterns/{filename}", "w") as f:
        for row in grid:
            f.write(" ".join(map(str, row)) + "\n")

# 1. Glider (Планер — двигающаяся фигура)
save_pattern("pattern1_glider.txt", [(1, 2), (2, 3), (3, 1), (3, 2), (3, 3)])

# 2. Gosper Glider Gun (Ружье Госпера — генератор планеров)
gun = [
    (5, 1), (5, 2), (6, 1), (6, 2),
    (5, 11), (6, 11), (7, 11), (4, 12), (8, 12), (3, 13), (9, 13),
    (3, 14), (9, 14), (6, 15), (4, 16), (8, 16), (5, 17), (6, 17), (7, 17),
    (6, 18), (3, 21), (4, 21), (5, 21), (3, 22), (4, 22), (5, 22),
    (2, 23), (6, 23), (1, 25), (2, 25), (6, 25), (7, 25),
    (3, 35), (4, 35), (3, 36), (4, 36)
]
save_pattern("pattern2_gun.txt", gun)

# 3. Pulsar (Пульсар — период 3, осциллятор)
pulsar = []
lines = [-6, -1, 1, 6]
cols = [-4, -3, -2, 2, 3, 4]
cr, cc = 12, 40
for r in lines:
    for c in cols:
        pulsar.append((cr + r, cc + c))
for r in cols:
    for c in lines:
        pulsar.append((cr + r, cc + c))
save_pattern("pattern3_pulsar.txt", pulsar)

# 4. Acorn (Желудь — долгожитель, Methuselah, живет более 5000 поколений)
acorn = [(12, 39), (14, 38), (14, 39), (14, 42), (14, 43), (14, 44), (13, 41)]
save_pattern("pattern4_acorn.txt", acorn)

# 5. LWSS (Легкий космический корабль)
lwss = [
    (10, 2), (10, 5),
    (11, 6),
    (12, 2), (12, 6),
    (13, 3), (13, 4), (13, 5), (13, 6)
]
save_pattern("pattern5_lwss.txt", lwss)

print("Patterns successfully generated in patterns/")
