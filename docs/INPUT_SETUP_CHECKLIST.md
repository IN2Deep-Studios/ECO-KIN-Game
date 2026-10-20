# Tamer Input Setup Checklist (5 minutes, in-editor)

The C++ is complete — the character binds 6 input actions and the controller loads a mapping
context. What is missing are the assets themselves. Create them once and the Tamer moves.

## 1. Create the Input Actions
Content Browser → right-click → Input → Input Action. Create in `Content/Input/`:

| Asset      | Value Type      |
|------------|-----------------|
| IA_Move    | Axis 2D (Vector2D) |
| IA_Look    | Axis 2D (Vector2D) |
| IA_Jump    | Digital (bool)  |
| IA_Interact| Digital (bool)  |
| IA_Scan    | Digital (bool)  |
| IA_Dodge   | Digital (bool)  |

## 2. Create the Mapping Context
Right-click → Input → Input Mapping Context → name it `IMC_Tamer`. Add:

| Action      | Keys |
|-------------|------|
| IA_Move     | W / A / S / D (Up/Down/Left/Right), Gamepad Left Stick |
| IA_Look     | Mouse X/Y (2D axis), Gamepad Right Stick |
| IA_Jump     | Space Bar, Gamepad Face Button Bottom |
| IA_Interact | E, Gamepad Face Button Top |
| IA_Scan     | Q, Gamepad Left Shoulder |
| IA_Dodge    | Left Shift, Gamepad Right Shoulder |

## 3. Wire the Blueprints
- Open your Tamer character Blueprint (child of `FrequencyTamerCharacter`) → Class Defaults →
  assign IA_Move → MoveAction, IA_Look → LookAction, IA_Jump → JumpAction,
  IA_Interact → InteractAction, IA_Scan → ScanAction, IA_Dodge → DodgeAction.
- Open your Tamer PlayerController Blueprint → Class Defaults →
  assign IMC_Tamer → TamerMappingContext.

## 4. Play
WASD moves, mouse looks, Space jumps, E interacts, Q scans (A.E.G.I.S.),
Shift dodges. If the character still does not move, the build itself has an
error — send it back and it gets fixed.
