Import("env")

# print(env.Dump())
env.Append(CCFLAGS=["-gdwarf-4"])
env.Append(LINKFLAGS=["-gdwarf-4"])

# print("Current CLI targets", COMMAND_LINE_TARGETS)
# print("Current Build targets", BUILD_TARGETS)
# def post_program_action(source, target, env):
#     print("Program has been built!")
#     program_path = target[0].get_abspath()
#     print("Program path", program_path)
#     if env.Execute(f"./scripts/with-venv.sh scrutiny get-firmware-id {program_path} --apply --output workfolder"):
#         env.Exit(1)
#     if env.Execute(f"./scripts/with-venv.sh scrutiny make-metadata --version='0.0.1' --author='ScrutinnyDevs' --project-name='ArduinoConsole' --output workfolder"):
#         env.Exit(1)
#     if env.Execute(f"./scripts/with-venv.sh scrutiny elf2varmap {program_path} --output workfolder"):
#         env.Exit(1)
#     if env.Execute(f"./scripts/with-venv.sh scrutiny make-sfd workfolder arduinoconsole-0.0.1.sfd"):
#         env.Exit(1)
#     if env.Execute(f"./scripts/with-venv.sh scrutiny install-sfd arduinoconsole-0.0.1.sfd"):
#         env.Exit(1)
# env.AddPostAction("$PROGPATH", post_program_action)
