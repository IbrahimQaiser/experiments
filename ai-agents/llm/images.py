from openai import OpenAI
import os
from dotenv import load_dotenv

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


response = client.responses.create(
    model=model_name,
    input=[
        {
            "role": "user",
            "content": [
                {"type": "input_text", "text": "what's in this image?"},
                {
                    "type": "input_image",
                    "detail": "low",
                    "image_url": "https://picsum.photos/200/300",
                },
            ],
        }  # type: ignore
    ],
)

print(response.output_text)
