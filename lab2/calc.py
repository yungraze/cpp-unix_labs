import time

def calculate(x):

    return x**2 - x**2 + x*4 - x*5 + x + x

def main():
    x = 10
    
    while True:
        user_input = input()

        if not user_input.isdigit():
            break
        
        n = int(user_input)
        
        start_time = time.time()
 
        res = 0

        for i in range(n):
            res = calculate(x)
            
        end_time = time.time()
        
        duration = end_time - start_time
        
        print(f"time = {duration} s; result(x=10) = {res}")

if __name__ == "__main__":
    main()