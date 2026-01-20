from openai import OpenAI
import os
from dotenv import load_dotenv

# Note: Only works with OpenAI (or AzureOpenAI), does not work with openai compatible endpoints since they may not implement
# store

load_dotenv()

if os.environ.get("API_BASE") is None:
    raise ValueError("x API_BASE environment variable not set")
if os.environ.get("API_KEY") is None:
    raise ValueError("x API_BASE environment variable not set")
if os.environ.get("MODEL") is None:
    raise ValueError("x MODEL environment variable not set")

api_base = os.environ["API_BASE"]
api_key = os.environ["API_KEY"]
model_name = os.environ["MODEL"]

client = OpenAI(
    base_url=api_base,
    api_key=api_key,
)

print(f"=== Using model {model_name} from {api_base} ===")
print(f"=== type 'exit' to exit the chat ===")

SYSTEM_MESSAGE = f"""
## START SYSTEM PROMPT
You are a very polite, helpful and knowledgable AI assistant. You answer everything the user asks for in a very polite and calm tone.
You explain things in detail, and do not hesitate in sharing information. Your purpose is to englighten the user and help them learn
what they want to
## END SYSTEM PROMPT
"""

response = client.responses.create(
    model=model_name,
    input=[
        {
            "role": "system",
            "content": SYSTEM_MESSAGE,
        }
    ],
    max_output_tokens=2000,
    temperature=0.5,
    store=True
)
previous_response_id = response.id


cumul_input = 0
cumul_output = 0

while True:
    line = input("> ")
    if line.strip() == "exit":
        break
    response = client.responses.create(
        model=model_name,
        input=[{"role": "user", "content": line.strip()}],
        max_output_tokens=2000,
        temperature=0.5,
        store=True,
        previous_response_id=previous_response_id,
    )
    previous_response_id = response.id
    print(response.output_text)
    if response.usage:
        output_tokens = response.usage.output_tokens
        total_tokens = response.usage.total_tokens
        input_tokens = response.usage.input_tokens
        print(
            f"[ input: {input_tokens}, output: {output_tokens}, total: {total_tokens} ]"
        )
        cumul_input += input_tokens
        cumul_output += output_tokens
        print(
            f"[ cumul_input: {cumul_input}, cumul_output: {cumul_output}, cumul_total: {cumul_input + cumul_output} ]"
        )
