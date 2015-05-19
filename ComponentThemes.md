# Introduction #

The Munin library uses "themes" in order to customize its appearance.  A theme is a collection of string->any key-value pairs, which is referred to as a Style.  This allows an implementation to either adopt the default UI theme, to change the UI piecemeal, or even to allow users to change the look and feel of their UI at runtime.

Consider the humble OK button.  Alternate renderings of this might be:

```
  ╔══════╗ ┌──────┐ ╒══════╕ ╓──────╖
  ║  OK  ║ │  OK  │ │  OK  │ ║  OK  ║ «  OK  »
  ╚══════╝ └──────┘ ╘══════╛ ╙──────╜
```

And so on, with an array of possible colours.

To enable this, there may be a style for the "button" component.  There would be styles for attributes in certain states:

  * `theme["button.foreground_colour"] -> attribute::colour`  (e.g. white)
  * `theme["button.intensity"] -> intensity` (e.g. intensity::normal)
  * `theme["button.selected.polarity"] -> polarity` (e.g. polarity::negative)

And also for the border style

  * `theme["button.border_style"] -> string` (e.g. "double", "single", "hmixed", "vmixed", "none", "default", "custom")

Except for "custom" and "default", this would look at the styles that begin "border." and use their styles for the rest of its look and feel.  "custom" would indicate to use styles that begin "button.border." instead, and "default" would just go to the system's default styles, whatever they are.

Standard components will list their available styles publicly.