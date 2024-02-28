# CSE3150 - Lab 5

`make all` to compile both testing and `main()` binaries.

`make check` to compile and run tests.

`make run` to compile and run `main()`.

# Comma-separated graph (csg) format

To ease the input of graphs into the program - and to have a little fun - I created an extremely simple graph description language to describe labelled digraphs with edge weights of {1, -1}.

## Description

A `.csg` file consists of one or more lines, each consisting of a comma-separated list of numeric vertex labels alternating with edge weights.

A `.csg` file describes a single graph.

Vertex labels cannot be negative, and graph order is defined as `max(V) - min(V)` for a vertex set `V`.

Ordering and line breaks do not affect the parsed graph, thus:

```
0,1,1
1,-1,2
```
and
```
0,1,1,-1,2
```

specify the same graph.

The symbols `+` and `-` may be used in lieu of comma-separators and explicit
edge weights. `{+, -}` maps to `{1, -1}`.

```
0,1,1,-1,2
```
and
```
0+1-2
```

specify the same graph.

## Specification

```ebnf
newline = ? U+000A ? .

nonzero_digit = "1" … "9" .
digit = "0" | nonzero_digit .
number = nonzero_digit { digit } .
nonnegative_integer = "0" | number .
integer = ["-"] nonnegative_integer .

vertex = nonnegative_integer .
edge_weight = "," integer "," .
alt_edge_weight = "+" | "-" .
edge = edge_weight | alt_edge_weight .

csg = line { newline line } [ newline ].

line = vertex edge vertex { edge vertex } .

```