import os
from dotenv import load_dotenv
from smolagents import CodeAgent, DuckDuckGoSearchTool, OpenAIModel, tool, ToolCallingAgent


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

model = OpenAIModel(
    api_base=api_base,
    api_key=api_key,
    model_id=model_name,
    temperature=0.5,
    max_tokens=1000,
)

print(f"=== Using model {model_name} from {api_base} ===")
print(f"=== type 'exit' to exit the chat ===")


@tool
def get_user_info() -> str:
    """
    Returns detailed information about the user who is operating this agent. Includes their likes/dislikes and what they do

    :return: Information about the user who is using this tool, each point starts with "-" in a fresh line
    :rtype: str
    """
    return f"""
    - Likes programming
    - Has worked with C++, Typescript, Python, Golang
    - Is currently learning about LLMs
    """


agent = CodeAgent(tools=[DuckDuckGoSearchTool(), get_user_info], model=model, stream_outputs=True, additional_authorized_imports=['decimal'])
while True:
    line = input("> ")
    if line.strip() == "exit":
        break
    agent.run(line)