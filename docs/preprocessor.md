# Preprocessor

Even though named as `preprocessor`, these don't work entirely like in C, C++ or C# 
They are parsed in the same process as everything else but handled way before everything else.

The directives are called `preprocesor`-directives because they look like the ones in C/C++ and C# 
due to the way the are written down

- [Preprocessor directives](#preprocessor-directives)
  - [Conditional Logic](#conditional-logic)
  - [File Inclusion](#file-inclusion)
  - [Platform Detection](#platform-detection)
  - [Raw Strings](#raw-strings)
  - [Logging](#logging)
  - [Miscellaneous](#miscellaneous)

## Preprocessor directives

Preprocessor directives are defined: `#{DIRECTIVE}`. 
All directives start with `.`.

### Conditional Logic


- Predefined values:
    - Separated into three groups:
        - Platform. If one of the following platforms are used, the value will be `TRUE`, `FALSE` otherwise:
            - `LINUX`, `WINDOWS`, `MACOS`, `UNIX` 
        - Architecture. If one of the following architectures are used, the value will be `TRUE`, `FALSE` otherwise:
            - `X86`, `X64`, `ARM`, `ARM64` 
        - Boolean:
            - `FALSE`, `TRUE`
- Logical operators. Value can be: Platform, Architecture, Boolean, Number, String, Variable
    - `OR`. OR, same as `||`
        - Usage: {VALUE} `OR` {VALUE}
    - `AND`. AND, same as `&&`
        - Usage: {VALUE} `AND` {VALUE}
    - `NOT`. NOT, same as `!`
        - Usage: `NOT` {VALUE}
    - `EQU`. EQUals, same as `==`
        - Usage: {VALUE} `EQU` {VALUE}
    - `NEQ`. Not EQual, same as `!=`
        - Usage: {VALUE} `NEQ` {VALUE}
    - `GT`. Greater Than, same as `>`
        - Usage: {VALUE} `GT` {VALUE}
    - `LT`. Less Than, same as `<`
        - Usage: {VALUE} `LT` {VALUE}
    - `GTE`. Greater than equal, same as `>=`
        - Usage: {VALUE} `GTE` {VALUE}
    - `LTE`. Less Than Equal, same as `<=`
        - Usage: {VALUE} `LTE` {VALUE}

- .IF {CONDITION}
- .ELSEIF {CONDITION}
- .ELSE
- .ENDIF

#### Example
```ini
#!ATRC

#.IF WINDOWS
%Latency%=100
#.ELSEIF LINUX
%Latency%=90
#.ENDIF

[HardwareInfo]
#.IF %Latency% GR 90
MaxLatency=%Latency%+10
#.ELSE
MaxLatency=%Latency%+20
#.ENDIF
# MaxLatency's result can be now calculated with the atrc_stdlib's `atrc_math_exp`. See stdlib_api.md for more information
```

### File Inclusion

The `.INCLUDE` directive will parse the file included and add 
all the block, key and variable definitions into the current file

- .INCLUDE {path/to/file}

#### Example

```ini
#!ATRC
#.INCLUDE ./globals.atrc
# Global values are now included into this file

[FilesystemInfo]
# Variable %EXT% is defined in the globals.atrc
MainFilesystem=Filesys.%EXT%
```

### Raw Strings

Can be used to define a raw string, either as a variable (VAR) or as a key (KEY). 
The name `Raw string` is misleading, as the value will be parsed the same way a normal 
value is, but this allows for multi-line values. See examples below

- .SR {KEY|VAR}
    - Denotes the starting of a raw string. 
        `KEY` denotes that the value will be given to the first key inside inside the directive. 
        `VAR` denotes that the value will be given to the first variable inside inside the directive. 
- .ER
    - Denotes the ending of a raw string.

#### Example
```ini
#!ATRC
#.SR VAR
%RawStringVar%=Raw string
variable definition will include
[ThisBlockToo]
#.ER

[RawStringTest]
#.SR KEY
RawString=Everything inside
the directive will be part of
the value
Including=This one!
#.ER
```


### Logging

Can be used to log a message inside ATRC file

- .LOG {MESSAGE}
    - Logs the message into standard output in white colour
- .WARNING {MESSAGE}
    - Logs the message into standard output in yellow colour
- .ERROR {MESSAGE}
    - Logs the message into standard output in red colour
- .ERRORCOUT {MESSAGE}
    - Logs the message into error output in red colour
- .SUCCESS {MESSAGE}
    - Logs the message into standard output in green colour
- .DEBUG {MESSAGE}
    - Logs the message into standard output in cyan colour

#### Example

```ini
#!ATRC
[SysInfo]
#.IF WINDOWS
OS=WINDOWS
#.ELSEIF LINUX OR UNIX
OS=LINUX
#.ELSE
#.ERRORCOUT Undefined platform
#.ENDIF
```

### Miscellaneous

- .DEFINE {NAME}={VALUE}
    - For now, definitions can be defined but not accessed. It will be added in version `2.3`.
- .UNDEF {NAME}
    - Undefines a defined value.
- .IGNORE {NUM}
    - Ignores the following `NUM` amount of rows.

#### Example

```ini
#!ATRC
#.DEFINE DEFINITION=TRUE
#.UNDEF DEFINITION
#.IGNORE 2
These lines will
be ignored by the parser
```