from openai import OpenAI
import os
from pydantic import BaseModel
from datetime import datetime
from dotenv import load_dotenv
import sys

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


class WeatherParameters(BaseModel):
    """
    Finds parameters such as temperature and humidity from natural text
    """

    is_valid: bool
    temperature: float | None
    humidity: float | None
    date: datetime | None


response = client.responses.parse(
    model=model_name,
    input=[
        {
            "role": "system",
            "content": "You are a structured content parser. Extract weather information from natural language. Unit of temperature is celsius"
            "If the input does not contain actual weather data, set is_valid=false and leave all other fields as null. "
            "DO NOT make up or hallucinate values - if the data is not present, mark it as invalid.",
        },
        {
            "role": "user",
            "content": "Current weather condition is Sunny with temperature (25C), humidity 28%, wind 6.1km/h. Updated on 20th Jan 2026",
        },
    ],
    text_format=WeatherParameters,
)

result = response.output_parsed
if result is None:
    print("Did not receive a response")
    exit(1)

if not result.is_valid:
    print(f"❌ Failed to extract valid weather data from input", file=sys.stderr)
    print(
        f"The input does not contain extractable weather information",
        file=sys.stderr,
    )
    sys.exit(1)

print(f"Temperature: {result.temperature}")
print(f"Humidity: {result.humidity}")
print(f"Date: {result.date}")