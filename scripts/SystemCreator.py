import json
import itertools
import string

def generate_variable_name(var_index, automaton_index):
    # Convert indexes to letters, allowing combinations of letters for large indexes
    def index_to_letters(index):
        result = ""
        while index >= 0:
            result = chr(index % 26 + ord('a')) + result
            index = index // 26 - 1
        return result

    return f"var{index_to_letters(var_index)}automa{index_to_letters(automaton_index)}"

def generate_automata(num_automata, num_states):
    system = {
        "system": {
            "automata": [],
            "global": {
                "delta": "0.2500",
                "finaltime": "100"
            }
        }
    }

    for i in range(num_automata):
        automaton_name = f"automa{generate_variable_name(0, i)}"
        automaton = {
            "name": automaton_name,
            "node": [],
            "variables": []
        }

        # Create variables for the automaton
        for v in range(num_states):
            variable_name = generate_variable_name(v, i)
            automaton["variables"].append({
                "name": variable_name,
                "value": "0"
            })

        # Create states and transitions for the automaton
        for s in range(num_states):
            # Generate the instructions for the state
            instructions = "; ".join([f"{generate_variable_name(v, i)} = {generate_variable_name(v, i)} + 1" for v in range(num_states)])
            instructions += ";"  # Ensure a semicolon at the end

            state_name = f"State{s+1}_{automaton_name}"

            state = {
                "description": "default description",
                "flag": "start" if s == 0 else "none",
                "instructions": instructions,
                "name": state_name,
                "transitions": [],
                "x": 100 * (s+1),  # Example coordinates, adjust as needed
                "y": 100 * (s+1)
            }

            for t in range(num_states):
                if s != t:
                    transition = {
                        "condition": f"({generate_variable_name(0, i)} % {num_states} == {t})",
                        "to": f"State{t+1}_{automaton_name}"
                    }
                    state["transitions"].append(transition)

            automaton["node"].append(state)
        
        system["system"]["automata"].append(automaton)

    return system

num_automata = 50
num_states = 20
automata_system = generate_automata(num_automata, num_states)

# Convert the system to JSON format
automata_system_json = json.dumps(automata_system, indent=4)

# Write to a JSON file
with open('generated_automata_system.json', 'w') as f:
    f.write(automata_system_json)

print(automata_system_json)
