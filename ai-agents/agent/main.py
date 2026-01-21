from dotenv import load_dotenv

load_dotenv()

from openai import OpenAI
from openai.types.responses import (
    ResponseInputItemParam,
)
import os
from planner import Planner
from executor import Executor, ExecutionResult, ToolCallsRequest
from tools import functions_list, tools
from typing import Any, Dict, List
from utils import print_tokens_used, print_plan
import json

if os.environ.get("API_BASE") is None:
    raise ValueError("x API_BASE environment variable not set")
if os.environ.get("API_KEY") is None:
    raise ValueError("x API_BASE environment variable not set")
if os.environ.get("MODEL") is None:
    raise ValueError("x MODEL environment variable not set")

MAX_TOOL_TURNS = 8


def main():
    api_base = os.environ["API_BASE"]
    api_key = os.environ["API_KEY"]
    model_name = os.environ["MODEL"]

    client = OpenAI(
        base_url=api_base,
        api_key=api_key,
    )

    planner = Planner(client=client)
    executor = Executor(client=client)

    print(f"=== Using model {model_name} from {api_base} ===")

    task = input("Task > ")
    run(planner, executor, task)


def run(planner: Planner, executor: Executor, task: str):
    plan = planner.plan(task, max_steps=50)
    if plan is None:
        print("Could not generate plan")
        exit(1)
    print_plan(plan)

    print(f"=== Execution ===")
    for i, step in enumerate(plan.steps):
        print(f"== Step {i + 1} ==")
        print(f"Step: {step}")
        turns = 0
        step_memory: List[ResponseInputItemParam] = []
        while True:
            if turns >= MAX_TOOL_TURNS:
                # TODO: Later do graceful handling of error
                raise RuntimeError(
                    f"Step {i+1} '{step}' exceeded tool call limit ({MAX_TOOL_TURNS})"
                )
            turns += 1
            result, output = executor.execute(step_memory, step, tools=tools)
            if result is None:
                print(f"=== WARNING (result is None) ===")
                break
            step_memory.extend(output)
            if isinstance(result, ToolCallsRequest):
                # Execute all required tools (TODO: Can do this in parallel, for now, this is done serially one by one)
                tool_outputs: list[ResponseInputItemParam] = []

                print(f"== Model requested {len(result.tool_calls)} tool calls ==")
                for tool_call in result.tool_calls:
                    func = functions_list.get(tool_call.name)
                    function_result = ""
                    if func is None:
                        function_result = f"Tool {tool_call.name} does not exist\n"
                    else:
                        try:
                            function_result = func(json.loads(tool_call.arguments))
                        except Exception as e:
                            function_result = (
                                f"Error: {tool_call.name} returned error: {e}"
                            )
                    print(
                        f"== Executed tool call {tool_call.name}({tool_call.arguments}) = '{function_result}' =="
                    )
                    function_call_output: ResponseInputItemParam = {
                        "type": "function_call_output",
                        "call_id": tool_call.call_id,
                        "output": json.dumps({"result": function_result}),
                    }
                    tool_outputs.append(function_call_output)
                step_memory.extend(tool_outputs)
                #print("###", step_memory)
            elif isinstance(result, ExecutionResult):
                #print("***", step_memory)
                print(f"=== Result of step {i+1} ===")
                print(f"Result: {result.result}")
                print(f"Observation: {result.observation}")
                print()
                break

    print_tokens_used(planner, executor)


if __name__ == "__main__":
    main()
