class Solver:
    def __init__(self):
        self.string = ''

    def can_solve(self):
        for char in self.string:
            if char in ['+', '-', '*', '/', '^']:
                return True
        return False

    def find_elements(self):
        for i in range(len(self.string)):
            if self.string[i] in ['+', '-', '*', '/', '^']:
                index = i
        try:
            a = int(self.string[self.find_first(self.string):index])
            b = int(self.string[index + 1:-1])
            op = self.string[index]
        except ValueError:
            a, b, op = (-1, -1, '+')
        return (a, b, op)

    def find_first(self, value):
        numbers = ['0', '1', '2', '3', '4', '5', '6', '7', '8', '9']
        for i in range(0, len(value)):
            if value[i] not in numbers:
                pass
            else:
                return i
        return -1

    def solve(self):
        if self.can_solve():
            a, b, op = self.find_elements()

            if a != -1:
                if op == '+':
                    return a + b
                elif op == '-':
                    return a - b
                elif op == '*':
                    return a * b
                elif op == '/':
                    return a / b
                elif op == '^':
                    return a ** b
            else:
                return 'Error. No es una operación'
        else:
            return 'Error. No es una operación'
