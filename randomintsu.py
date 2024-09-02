import random

NUM = 100
# Generate 10 million random numbers
numbers = [str(random.randint(1, NUM)) for _ in range(NUM)]

# Join the numbers with commas
numbers_str = ','.join(numbers)

# Write the numbers to a text file
with open('random_numbers.txt', 'w') as file:
    file.write(numbers_str)