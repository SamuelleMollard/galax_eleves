from math import sqrt
import random


def counted(f):
    def wrapped(*args, **kwargs):
        wrapped.calls += 1
        print(wrapped.calls / 10000)
        return f(*args, **kwargs)
    wrapped.calls = 0
    return wrapped

class Octree():
    def __init__(self):
        pass

    def make_tree(cube, universe):
        if cube[3] < 0.01:
            mass = 0
            pos = [0, 0, 0]
            for part in universe:
                if part in cube:
                    mass += part.mass
                    pos[0] += part.pos[0] * part.mass
                    pos[1] += part.pos[1] * part.mass
                    pos[2] += part.pos[2] * part.mass
            if mass == 0:
                return InternalNode(cube, None)
            pos[0] /= mass
            pos[1] /= mass
            pos[1] /= mass
            return InternalNode(cube, Particle(pos[0], pos[1], pos[2], 0, 0, 0, 0, 0,
                0, mass))
        is_full, part = Octree.full(cube, universe)
        if is_full:
            return InternalNode(cube, part)
        else:
            return ExternalNode(cube, universe)

    def full(cube, universe):
        nb_particles = 0
        found_particles = None
        for part in universe:
            if part.in_cube(cube):
                nb_particles += 1
                found_particles = part
                if nb_particles >= 2:
                    return (False, 0)
        return (True, found_particles)

class ExternalNode(Octree):
    def __init__(self, cube, universe):
        left, top, fg, width  = cube
        w = width/2
        self.children = [Octree.make_tree((left, top, fg, w), universe),
                Octree.make_tree((left + w, top, fg, w), universe),
                Octree.make_tree((left, top + w, fg, w), universe),
                Octree.make_tree((left + w, top + w, fg, w), universe),
                Octree.make_tree((left, top, fg + w, w), universe),
                Octree.make_tree((left + w, top, fg + w, w), universe),
                Octree.make_tree((left, top + w, fg + w, w), universe),
                Octree.make_tree((left + w, top + w, fg + w, w), universe)]
        self.mass = sum([self.children[i].mass for i in range(8)])
        self.posx = sum([self.children[i].mass * self.children[i].pos[0] for i
            in range(8)]) / 8
        self.posy = sum([self.children[i].mass * self.children[i].pos[1] for i
            in range(8)]) / 8
        self.posz = sum([self.children[i].mass * self.children[i].pos[1] for i
            in range(8)]) / 8
        self.center_of_mass = [self.posx, self.posy, self.posz]
        self.pos = self.center_of_mass

    def node_type(self):
        return 1

class InternalNode(Octree):
    def __init__(self, cube, particle):
        if particle == None:
            self.mass = 0
            self.center_of_mass = [0, 0, 0]
            self.pos = [0, 0, 0]
        else:
            self.particle = particle
            self.cube = cube
            self.center_of_mass = particle.pos
            self.mass = particle.mass
            self.pos = self.center_of_mass  # Used to ensure compatibility with
                                            # Particle.dist
            self.count()
    
    @counted
    def count(self):
        pass

class Particle():
    def __init__(self, x, y, z, vx, vy, vz, ax, ay, az, mass):
        self.pos = [x, y, z]
        self.speed = [vx, vy, vz]
        self.acc = [ax, ay, az]
        self.mass = mass

    def dist(self, other):
        return sqrt((self.x - other.x)**2 + (self.y - other.y)**2 + (self.z -
            other.z)**2)

    def in_cube(self, cube):
        return (self.pos[0] >= cube[0] and self.pos[0] < cube[0] + cube[3]
                and self.pos[1] >= cube[1] and self.pos[1] < cube[1] + cube[3]
                and self.pos[2] >= cube[2] and self.pos[2] < cube[2] + cube[3])

def create_universe():
    universe = []
    for _ in range(10000):
        x = random.uniform(0, 1)
        y = random.uniform(0, 1)
        z = random.uniform(0, 1)
        vx = random.uniform(0, 1)
        vy = random.uniform(0, 1)
        vz = random.uniform(0, 1)
        ax = random.uniform(0, 1)
        ay = random.uniform(0, 1)
        az = random.uniform(0, 1)
        mass = random.uniform(0, 1)
        universe.append(Particle(x, y, z, vx, vy, vz, ax, ay, az, mass))
    return universe

def main():
    random.seed(42)
    universe = create_universe()
    tree = Octree.make_tree((0, 0, 0, 1), universe)
    import pdb; pdb.set_trace()
