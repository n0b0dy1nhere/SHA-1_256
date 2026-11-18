import hashlib

user_input = input("Enter input: ")
result = hashlib.sha256(user_input.encode("utf-8")).hexdigest()
print("SHA-256:", result)