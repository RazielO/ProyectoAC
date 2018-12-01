class Solver:
    def __init__(self):
        """
        Constructor, se inicializa la cadena vacía
        """

        self.string = ''

    def can_solve(self):
        """
        Método que busca un operando en la cadena para saber
        si se puede resolver
        """

        for char in self.string:  # Iterar a traves de la cadena
            if char in ['+', '-', 'X', '/', 'P']:  # Si se encuentra un operando
                return True  # Se puede resolver
        return False  # No se puede resolver

    def find_elements(self):
        """
        Método que busca los números y el operando
        """

        for i in range(len(self.string)):  # Iterar a lo largo de la cadena
            if self.string[i] in ['+', '-', 'X', '/', 'P']:  # Si encuentra un operando
                index = i  # Se da el índice del operando

        a = int(self.string[self.find_first():index])  # Primer número
        b = int(self.string[index:self.find_parentheses()])  # Segundo número
        op = self.string[index]  # Operando

        return (a, b, op)  # Regresa una tupla inmutable con los números y el operando

    def find_first(self):
        """
        Método que regresa la posición del primer caracter
        que no sea un número
        """

        for i in range(0, len(self.string)):  # Iterar a lo largo de la cadena
            try:  # Intenta
                int(self.string[i])  # Convertir el caracter en un entero
                return i  # Regresa la posicion del primer número
            except ValueError:  # Si no es número
                pass  # Continua en el siguiente caracter
        return -1  # Regresa -1

    def find_parentheses(self):
        """
        Método que busca el último parentesis y regresa la 
        posición del parentesis
        """

        index = 0  # Indice del parentesis
        for i in range(0, len(self.string)):  # Iterar a lo largo de la cadena
            if self.string[i] == ')':  # Si encuentra ')'
                index = i  # Hacer el indice igual a i
        return index  # Regresar el indice del parentesis

    def solve(self):
        """
        Método que resuelve la operación dada
        """

        if self.can_solve():  # Si se puede resolver
            a, b, op = self.find_elements()  # Encontrar los números y el operando

            # Resuelve la operación
            if op == '+':  # Suma
                return a + b
            elif op == '-':  # Resta
                return a - b
            elif op == 'X':  # Multiplicacion
                return a * b
            elif op == '/':  # Division
                return a / b
            elif op == 'P':  # Potencia
                return a ** b
        else:  # Si no se puede resolver
            return 'Error'  # Regresa 'Error'
