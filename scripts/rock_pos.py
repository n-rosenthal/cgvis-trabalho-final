import math
import numpy as np

# Your 29 points
points = np.array([
    [-235, 70], [-244, 70], [-265, 40], [-271, 15], [-269, 21],
    [-268, 35], [-263, 49], [-258, 58], [-238, -88], [-232, 90],
    [-198, -115], [-168, -134], [-129, -151], [-107, -153], [-98, -153],
    [-60, -169], [-174, -170], [-23, -162], [5, -167], [13, -171],
    [40, -170], [73, -161], [100, 159], [131, -142], [161, -129],
    [188, -120], [216, -104], [238, -86], [263, -61]
])

# 1. Fit ellipse
mean = np.mean(points, axis=0)
centered = points - mean
cov = np.cov(centered, rowvar=False)
eigvals, eigvecs = np.linalg.eigh(cov)
# sort descending
idx = np.argsort(eigvals)[::-1]
eigvals = eigvals[idx]
eigvecs = eigvecs[:, idx]

# Semi-axes (use 2*sigma to cover spread)
a = 2.0 * math.sqrt(eigvals[0])
b = 2.0 * math.sqrt(eigvals[1])
theta = math.atan2(eigvecs[1,0], eigvecs[0,0])  # orientation of major axis

# 2. Generate points on ellipse at evenly spaced angles (say 50)
N_ellipse = 50
angles = np.linspace(0, 2*math.pi, N_ellipse, endpoint=False)
ellipse_pts = []
for ang in angles:
    # local ellipse coords
    ex = a * math.cos(ang)
    ey = b * math.sin(ang)
    # rotate and translate
    x = mean[0] + ex * math.cos(theta) - ey * math.sin(theta)
    y = mean[1] + ex * math.sin(theta) + ey * math.cos(theta)
    ellipse_pts.append((x, y))

# 3. Merge given points (ensure they are included)
all_pts = list(points)  # given points (as tuples)
# Add ellipse points if they are not too close to an existing given point
tolerance = 2.0  # distance threshold to avoid duplicates
for ex, ey in ellipse_pts:
    if not any(math.hypot(ex - gx, ey - gy) < tolerance for gx, gy in all_pts):
        all_pts.append((ex, ey))

# 4. Sort points by angle around the centroid (to get a nice order)
def angle_from_center(p):
    dx = p[0] - mean[0]
    dy = p[1] - mean[1]
    return math.atan2(dy, dx)

all_pts.sort(key=angle_from_center)

# 5. Assign textures and scales in a repeating pattern
textures = ["&Assets::ROCK_1", "&Assets::ROCK_2", "&Assets::ROCK_3", "&Assets::ROCK_4"]
scales = [0.25, 0.15, 0.10, 0.07]

# Build the C++ initializer list
print("const std::vector<StaticObjectDef> rocks = {")
for i, (x, y) in enumerate(all_pts):
    tex = textures[i % 4]
    scale = scales[i % 4]
    # format with 2 decimals
    print(f"    {{ {tex}, {{ {x:.2f}f, {y:.2f}f }}, {{ 0.0f, 0.0f, 0.0f }}, {scale:.2f}f }},")
print("};")