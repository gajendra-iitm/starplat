class Node:
    def __init__(self, value):
        self.value = value
        

class PropNode:
    def __init__(self, value):
        self.value = value
        
    def __repr__(self):
        return '<Value: {}>'.format(self.value)