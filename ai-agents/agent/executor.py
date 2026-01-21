from openai import OpenAI
from typing import List, Any, Dict, Tuple
from pydantic import BaseModel
from openai.types.responses.tool_param import ParseableToolParam
from openai.types.responses import (
    ParsedResponseFunctionToolCall,
    ResponseInputItemParam,
)
import os

SYSTEM_PROMPT = """
You are an execution agent.

Your job is to execute exactly **one atomic step** and return a structured result.

Rules:
1. Execute only the given step. Do not plan ahead or infer future steps.
2. Do not include explanations, reasoning, commentary, or extra text outside the schema.
3. The output must **exactly match the schema**.
4. The schema is:

{
  "result": <the final result of this step>,
  "observation": <a short factual description of what was done, 1-2 sentences>
}

5. observation must be concise, factual, and describe only what was executed.
6. ExecutionResult must contain only the result of this step and the observation; nothing else.
7. If tools were used, do not include tool arguments or reasoning, just include the observed effect in observation and the step result in result.
8. Only use the outputs provided by any tool calls. Do NOT attempt to compute results yourself.
9. If a step has a tool call output, use the output of the tool as the 'result'.

Example valid output:

{
  "result": 42,
  "observation": "Calculated the sum of 20 and 22."
}

Always produce output in this exact JSON format. Do not include markdown, quotes, or any text outside the JSON.

"""


class ExecutionResult(BaseModel):
    """
    Results obtained after executing a step. It consists of a result that can be used for future execution, and observation about this execution
    """

    result: str
    observation: str


class ToolCallsRequest(BaseModel):
    """
    Object of this type is returned from execute if the step requires execution of one or more tool calls.
    Once all tool calls are executed, they must be passed back along with the step so that the model can produce
    the final result
    """

    tool_calls: List[ParsedResponseFunctionToolCall]


class Executor:
    """
    Executor executes a step one by one
    """

    def __init__(self, client: OpenAI, model: str = os.environ["MODEL"]) -> None:
        self.client = client
        self.model = model
        self.input_tokens = 0
        self.output_tokens = 0
        self.total_tokens = 0

    def execute(
        self,
        step_context: List[ResponseInputItemParam],
        step: str,
        temperature=0.0,
        max_output_tokens=150,
        tools: list[ParseableToolParam] | None = None,
    ) -> Tuple[ExecutionResult | ToolCallsRequest | None, Any]:
        # Call the api
        if tools is None:
            tools = []
        context: List[ResponseInputItemParam] = [
            {"role": "system", "content": SYSTEM_PROMPT},
            {"role": "user", "content": f"Step: {step}"},
        ]
        context += step_context
        response = self.client.responses.parse(
            input=context,
            model=self.model,
            max_output_tokens=max_output_tokens,
            temperature=temperature,
            store=False,
            text_format=ExecutionResult,
            tools=tools,
        )
        tool_calls: List[ParsedResponseFunctionToolCall] = []
        for item in response.output:
            if item.type == "function_call":
                tool_calls += [item]
        if response.usage:
            self.input_tokens += response.usage.input_tokens
            self.output_tokens += response.usage.output_tokens
            self.total_tokens += response.usage.total_tokens
        if tool_calls and response.output_parsed is not None:
            raise RuntimeError("Model returned both tool calls and parsed output")
        if tool_calls:
            return (ToolCallsRequest(tool_calls=tool_calls), response.output)
        return (response.output_parsed, response.output)
