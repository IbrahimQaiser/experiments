from openai.types.responses.tool_param import ParseableToolParam
from typing import Callable, Any, List

tools: List[ParseableToolParam] = [
    {
        "type": "function",
        "name": "calculate",
        "description": (
            "Computes the result of a mathematical expression. "
            "The expression will be passed to python eval(). "
            "IMPORTANT: The model must return the argument as valid JSON. "
            "Escape quotes and special characters. Do not return raw Python objects."
            "Available modules - math, numpy as np"
        ),
        "parameters": {
            "type": "object",
            "properties": {
                "expression": {
                    "type": "string",
                    "description": "the expression to be evaluated in python syntax",
                }
            },
            "required": ["expression"],
            "additionalProperties": False,
        },
        "strict": True,
    }
]

import math
import numpy as np


def calculate(obj: dict[str, str]) -> str:
    if obj.get("expression") is None:
        return f"Error: key 'expression' not present in argument"
    # TODO: UNSAFE
    try:
        result = str(eval(obj["expression"]))
        return result
    except Exception as e:
        return f"Error while invoking tool"


functions_list: dict[str, Callable[..., Any]] = {"calculate": calculate}
