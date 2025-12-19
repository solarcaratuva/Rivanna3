# Rivanna3
Codebase for the embedded code that runs on the Solar Car. Rivanna3 design cycle. <br>
This code was made by and for the Solar Car Team at UVA. Use elsewhere is not recommended. 

## Resources on the Team Wiki

**Warning**: The team wiki has been updated for the current design cycle. See `compile_upload.md` and `dev_guide.md` in this repo for archived versions of those pages that are accurate for *Rivanna3*. 

Team Wiki: [https://solarcaratuva.github.io/](https://solarcaratuva.github.io/) <br>

**Compiling**: [Embedded >> Rivanna3 Compile & Upload](https://solarcaratuva.github.io/Embedded/riv3_compile_upload.html) <br>
**Uploading**: [Embedded >> Rivanna3 Compile & Upload](https://solarcaratuva.github.io/Embedded/riv3_compile_upload.html) <br>
**Development Guide**: [Embedded >> Rivanna3 Development Guide](https://solarcaratuva.github.io/Embedded/riv3_dev_guide.html)


## Serial Monitor and CAN Message Monitor

**Software Installation**

Install PySerial on your computer by running `py -m pip install pyserial`
- you may need to use `python` or `python3` instead of `py`

**Serial Monitor**

Made for basic monitoring, prints all log messages received over serial to the terminal

Attach an ST-Link to a board and your computer, then `cd` into this repo, then run `py monitor.py filepath`
- `filepath` is optional, if included it will save all log messages to the file
- you may need to use `python` or `python3` instead of `py`

**CAN Message Monitoring**

For CAN-based advanced logging, debugging, and analysis, see the [CAN Message Logger](https://github.com/solarcaratuva/CANMessageLogger) project.

## Development

*These are recommendations*
- View and edit code using [VSCode](https://code.visualstudio.com/)
    - install the extensions: `Remote Extensionpack`, `C++ Tools Extensionpack`, `Docker`, `Python`
    - Windows Users: use these extensions to *remote* into where this repo is stored in WSL
