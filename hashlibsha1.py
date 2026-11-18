import hashlib

user_input = input("Enter input: ")
digest = hashlib.sha1(user_input.encode("utf-8")).hexdigest()
print("SHA-1:", digest)