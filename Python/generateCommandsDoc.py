import sys
import os
import re
import argparse

PACKAGES_DIR=os.path.realpath(os.path.join(os.path.abspath(os.path.dirname(__file__)), '..', '..'))
sys.path.append('%(PACKAGES_DIR)s/ARSDKBuildUtils/Utils/Python' % locals())
sys.path.append('%(PACKAGES_DIR)s/libARCommands/Tools' % locals())
sys.path.append('%(PACKAGES_DIR)s/arsdk-xml' % locals())

import arsdkparser
from ARFuncs import *
from ARCommandsParser import *
from generateFeatureControllers import *
from generateDeviceControllers import *
from generateDictionaryKeyEnum import *

# Matches 111-22 or 111-222-3 between r'(#' and r')'
LINK_PATTERN = r'(?<=\(#)\d+(?:-\d+){1,2}(?=\))'

_LIST_FLAG = 'list_flags'

DEVICE_TO_STRING = {
    'drones': 'all drones',
    'rc':     'all remote controllers',
    'none':   'no product',
    '0900':   'Rolling Spider',
    '0901':   'Bebop',
    '0902':   'Jumping Sumo',
    '0903':   'SkyController',
    '0905':   'Jumping Night',
    '0906':   'Jumping Race',
    '0907':   'Airborne Night',
    '0909':   'Airborne Cargo',
    '090a':   'Hydrofoil',
    '090b':   'Mambo',
    '090c':   'Bebop 2',
    '090e':   'Disco',
    '090f':   'SkyController 2',
    '0910':   'Swing',
    '0913':   'SkyController 1 v2.x',
    '0915':   'SkyController 2P',
}
DEVICES_GLOBAL = [ 'drones', 'rc', 'none' ]
DEVICES_RC     = [ '0903', '090f', '0913', '0915' ]
DEVICES_DRONE  = [ x for x in DEVICE_TO_STRING if x not in (DEVICES_GLOBAL+DEVICES_RC) ]
# List of devices for which the doc should not be generated
DEVICES_IGNORED = [ '0913' ]

BG_BLUE = '\033[00;44m'
BLUE =    '\033[00;94m'
GREEN =   '\033[00;92m'
PURPLE =  '\033[00;95m'
RED =     '\033[00;91m'
WHITE =   '\033[00;0m'
YELLOW =  '\033[00;93m'


##########################################################
#   Utils functions
##########################################################
def _event_doc_file_name(feature):
    return '_events_' + feature.name + '.md'

def _command_doc_file_name(feature):
    return '_commands_' + feature.name + '.md'

def _parse_features(ctx, xmlFolder):
    # first load generic.xml
    parse_xml(ctx, os.path.join(xmlFolder, 'generic.xml'))
    for f in sorted(os.listdir(xmlFolder)):
        if not f.endswith('.xml') or f == 'generic.xml':
            continue
        parse_xml(ctx, os.path.join(xmlFolder, f))

def _format_message_name(feature, msg):
    msgName = get_ftr_old_name(feature) + '-'
    if msg.cls:
        msgName += msg.cls.name + '-'
    msgName += msg.name
    return msgName

def _is_command_supported(cmd, product):
    if product is None:
        return True
    try:
        devices = [x.partition(':')[0] for x in cmd.doc.support.split(';')]
        if 'drones' in devices:
            return product in DEVICES_DRONE
        elif 'rc' in devices:
            return product in DEVICES_RC
        elif 'none' in devices:
            return False
        else:
            return product in devices
    except:
        return False


# Get the message link name (for example ARDrone3-Piloting-FlatTrim)
# formatted_id: the message id as entered in the comments.
#            for example 1-2-9 for messages from projects
#            or 134-2 for messages from features
def _get_msg_name_from_formatted_id(ctx):
    def replace_id(formatted_id):
        part = formatted_id.group(0).split('-')
        project_id = int(part[0])
        feature = ctx.featuresById[int(project_id)]
        if len(part) == 2:
            message_id = int(part[1])
            msg = feature.getMsgsById()[message_id]
        elif len(part) == 3:
            class_id = int(part[1])
            message_id = int(part[2])
            project_class = feature.classesById[class_id]
            msg = project_class.cmdsById[message_id]
        return _format_message_name(feature, msg)
    return replace_id

# Get the support list as a formatted string
# supportStr: The support string as entered in the comments.
#             For example 0901;0902:3.2.0;090f
def _get_support_list_formatted(supportStr):
    supportListFormatted = ''
    deviceList = supportStr.split(';')
    for device in deviceList:
        deviceDesc = device.split(':')
        if deviceDesc[0] in DEVICES_IGNORED:
            continue
        supportListFormatted += '- *' + DEVICE_TO_STRING[deviceDesc[0]]
        if len(deviceDesc) > 1:
            supportListFormatted += ' since ' + deviceDesc[1]
        supportListFormatted += '*<br/>\n'

    if supportListFormatted:
        supportListFormatted += '\n\n'

    return supportListFormatted

# Return the given string where each id link (for example 134-2) is replaced by a link name
def _replace_links(ctx, text):
    return re.sub(LINK_PATTERN, _get_msg_name_from_formatted_id(ctx), text)

def _get_args_multiset(args):
    for arg in args:
        if isinstance(arg.argType, arsdkparser.ArMultiSetting):
            yield arg

def _get_msgs_without_multiset(msgs):
    for msg in msgs:
        if not list(_get_args_multiset(msg.args)):
            yield msg

##########################################################
#   Write common doc functions
##########################################################

def _write_message_header(docfile, feature, msg):
    docfile.write('<!-- ' + _format_message_name(feature, msg) + '-->\n')
    docfile.write('### <a name="' + _format_message_name(feature, msg) + '">')
    docfile.write(msg.doc.title)
    if msg.isDeprecated:
        docfile.write(' (deprecated)')
    docfile.write('</a><br/>\n')

def _write_message_code_header(docfile, msg):
    docfile.write('> ' + msg.doc.title)
    if msg.isDeprecated:
        docfile.write(' (deprecated)')
    docfile.write(':\n\n')

def _write_message_comment(ctx, docfile, msg, comment):
    if msg.isDeprecated:
        docfile.write('*This message is deprecated.*<br/>\n\n')
    for comment_line in comment.split('\n'):
        docfile.write(_replace_links(ctx, comment_line) + '<br/>\n')
    docfile.write('\n\n')

def _write_message_args(docfile, args):
    for arg in args:
        if arg.name == _LIST_FLAG:
            continue
        if isinstance(arg.argType, ArEnum):
            docfile.write ('* ' + arg.name + ' (' + 'enum' + '): ')
            for comm in arg.argType.doc.split('\n'):
                docfile.write (comm + '<br/>\n')

            for enum in arg.argType.values:
                docfile.write ('   * ' + enum.name + ': ')
                for enumCom in enum.doc.split('\n'):
                    docfile.write (enumCom + '<br/>\n')
        elif isinstance(arg.argType, ArBitfield):
            docfile.write ('* ' + arg.name + ' (' + 'bitfield as ' + ArArgType.TO_STRING[arg.argType.btfType] + '): ')
            for comm in arg.argType.enum.doc.split('\n'):
                docfile.write (comm + '<br/>\n')

            for enum in arg.argType.enum.values:
                docfile.write ('   * ' + enum.name + ': ')
                for enumCom in enum.doc.split('\n'):
                    docfile.write (enumCom + '<br/>\n')
        elif isinstance(arg.argType, ArMultiSetting):
            docfile.write ('* ' + arg.name + ' (multi setting): ')
            for comm in arg.argType.doc.split('\n'):
                docfile.write (comm + '<br/>\n')

            for msg in arg.argType.msgs:
                docfile.write ('   * [' + _format_message_name(msg.ftr, msg) + '](#' + _format_message_name(msg.ftr, msg) + '): ')
                docfile.write (msg.doc.title + '<br/>\n')
        else:
            docfile.write ('* ' + arg.name + ' (' + ArArgType.TO_STRING[arg.argType] + '): ')
            for comm in arg.doc.split('\n'):
                docfile.write (comm + '<br/>\n')

def _write_message_support(docfile, support):
    if support:
        docfile.write('\n\n')
        docfile.write('*Supported by <br/>*\n\n')
        docfile.write(_get_support_list_formatted(support))

##########################################################
#   Write commands doc functions
##########################################################

# write the documentation of all commands of the given feature
def _write_commands_doc(ctx, docfile, feature, args, product=None):
    ARPrint ('Feature ' + feature.name)

    nb_cmd = 0

    for cmd in feature.cmds:
        if not _is_command_supported(cmd, product):
            continue
        nb_cmd += 1

        _write_message_header(docfile, feature, cmd)

        _write_message_code_header(docfile, cmd)

        _write_command_c_code(docfile, feature, cmd)
        _write_command_objc_code(docfile, feature, cmd)
        _write_command_java_code(docfile, feature, cmd)

        _write_message_comment(ctx, docfile, cmd, cmd.doc.desc)

        _write_message_args(docfile, cmd.args)

        _write_command_result(ctx, docfile, cmd.doc.result)

        _write_message_support(docfile, cmd.doc.support)

        docfile.write('<br/>\n\n')

        if args.warning:
            warning_str = ""
            if not cmd.doc.support and not cmd.isDeprecated:
                warning_str += '{}{}{}'.format(RED, "- Support list is missing\n", WHITE)

            if not cmd.doc.result and not cmd.isDeprecated:
                warning_str += '{}{}{}'.format(RED, "- Result is missing\n", WHITE)

            if len(cmd.doc.title) > 35:
                warning_str += '{}{}{}'.format(BLUE, "- Title too long\n", WHITE)

            if cmd.doc.support == 'none':
                warning_str += '{}{}{}'.format(BLUE, "- Support is none\n", WHITE)

            if warning_str:
                print((_format_message_name(feature, cmd) + ":\n" + warning_str))
    return nb_cmd

def _write_command_c_code(docfile, feature, cmd):
    docfile.write('```c\n')
    docfile.write('deviceController->' + ARUncapitalize(get_ftr_old_name(feature)) + '->' + 'send' + ARCapitalize(format_cmd_name(cmd)) + '(deviceController->' + ARUncapitalize(get_ftr_old_name(feature)))
    for arg in cmd.args:
        docfile.write (', (' + xmlToC (MODULE_ARCOMMANDS, feature, cmd, arg) + ')' + arg.name)
    docfile.write(');\n')
    docfile.write('```\n\n')

def _write_command_objc_code(docfile, feature, cmd):
    docfile.write('```objective_c\n')
    docfile.write('deviceController->' + ARUncapitalize(get_ftr_old_name(feature)) + '->' + 'send' + ARCapitalize(format_cmd_name(cmd)) + '(deviceController->' + ARUncapitalize(get_ftr_old_name(feature)))
    for arg in cmd.args:
        docfile.write (', (' + xmlToC (MODULE_ARCOMMANDS, feature, cmd, arg) + ')' + arg.name)
    docfile.write(');\n')
    docfile.write('```\n\n')

def _write_command_java_code(docfile, feature, cmd):
    docfile.write('```java\n')
    docfile.write('deviceController.getFeature'+ ARCapitalize(get_ftr_old_name(feature)) + '().' + 'send' + ARCapitalize(format_cmd_name(cmd)) + '(')
    first = True
    for arg in cmd.args:
        if first:
            first = False
        else :
            docfile.write (', ')
        docfile.write ('(' + xmlToJava (MODULE_ARCOMMANDS, feature, cmd, arg) + ')' + arg.name + '')
    docfile.write(');\n')
    docfile.write('```\n')
    docfile.write('\n')

def _write_command_result(ctx, docfile, result):
    if result:
        docfile.write ('\n\nResult:<br/>\n')
        for result_line in result.split('\n'):
            docfile.write(_replace_links(ctx, result_line) + '<br/>\n')

##########################################################
#   Write events doc functions
##########################################################

# write the documentation of all events of the given feature
def _write_events_doc(ctx, docfile, feature, args, product=None):
    nb_evt = 0

    for evt in _get_msgs_without_multiset(feature.evts):
        if not _is_command_supported(evt, product):
            continue
        nb_evt += 1

        _write_message_header(docfile, feature, evt)

        _write_message_code_header(docfile, evt)

        if evt.listType == ArCmdListType.LIST or evt.listType == ArCmdListType.MAP:
            _write_event_list_c_code(docfile, feature, evt)
            _write_event_list_objc_code(docfile, feature, evt)
            _write_event_list_java_code(docfile, feature, evt)
        else:
            _write_event_c_code(docfile, feature, evt)
            _write_event_objc_code(docfile, feature, evt)
            _write_event_java_code(docfile, feature, evt)

        _write_message_comment(ctx, docfile, evt, evt.doc.desc)

        _write_message_args(docfile, evt.args)

        _write_event_triggered(ctx, docfile, evt.doc.triggered)

        _write_message_support(docfile, evt.doc.support)

        docfile.write('<br/>\n\n')

        if args.warning:
            warning_str = ""
            if not evt.doc.support and not evt.isDeprecated:
                warning_str += '{}{}{}'.format(RED, "- Support list is missing\n", WHITE)

            if not evt.doc.triggered and not evt.isDeprecated:
                warning_str += '{}{}{}'.format(RED, "- Triggered is missing\n", WHITE)

            if len(evt.doc.title) > 35:
                warning_str += '{}{}{}'.format(BLUE, "- Title too long\n", WHITE)

            if evt.doc.support == 'none':
                warning_str += '{}{}{}'.format(BLUE, "- Support is none\n", WHITE)

            if warning_str:
                print((_format_message_name(feature, evt) + ":\n" + warning_str))
    return nb_evt

def _write_event_list_c_code(docfile, feature, evt):
    docfile.write('```c\n')
    docfile.write('void onCommandReceived (eARCONTROLLER_DICTIONARY_KEY commandKey, ARCONTROLLER_DICTIONARY_ELEMENT_t *elementDictionary, void *customData)\n')
    docfile.write('{\n')
    docfile.write('    if (commandKey == '+defineNotification(feature, evt)+')\n')
    docfile.write('    {\n')
    docfile.write('        if (elementDictionary != NULL)\n')
    docfile.write('        {\n')
    if evt.args:
        docfile.write('            ARCONTROLLER_DICTIONARY_ARG_t *arg = NULL;\n')
        docfile.write('            ARCONTROLLER_DICTIONARY_ELEMENT_t *dictElement = NULL;\n')
        docfile.write('            ARCONTROLLER_DICTIONARY_ELEMENT_t *dictTmp = NULL;\n')
        docfile.write('            HASH_ITER(hh, elementDictionary, dictElement, dictTmp)\n')
        docfile.write('            {\n')
        # print all args except the list flags
        for arg in [arg_tmp for arg_tmp in evt.args if not arg_tmp.name == _LIST_FLAG]:
            docfile.write('                HASH_FIND_STR (dictElement->arguments, '+defineNotification(feature, evt, arg)+', arg);\n')
            docfile.write('                if (arg != NULL)\n')
            docfile.write('                {\n')
            docfile.write('                    '+xmlToC(MODULE_ARCOMMANDS, feature, evt, arg)+' '+arg.name+' = arg->value.')
            if isinstance(arg.argType, ArEnum):
                docfile.write (ARCapitalize('i32'))
            elif isinstance(arg.argType, ArBitfield):
                docfile.write (ARCapitalize(ArArgType.TO_STRING[arg.argType.btfType]))
            else:
                docfile.write (ARCapitalize(ArArgType.TO_STRING[arg.argType]))
            docfile.write(';\n')
            docfile.write('                }\n')
        docfile.write('            }\n')
    else:
        docfile.write('\n')
    docfile.write('        }\n')
    docfile.write('        else\n')
    docfile.write('        {\n')
    docfile.write('            // list is empty\n')
    docfile.write('        }\n')
    docfile.write('    }\n')
    docfile.write('}\n')
    docfile.write('```\n\n')

def _write_event_list_objc_code(docfile, feature, evt):
    docfile.write('```objective_c\n')
    docfile.write('void onCommandReceived (eARCONTROLLER_DICTIONARY_KEY commandKey, ARCONTROLLER_DICTIONARY_ELEMENT_t *elementDictionary, void *customData)\n')
    docfile.write('{\n')
    docfile.write('    if (commandKey == '+defineNotification(feature, evt)+')\n')
    docfile.write('    {\n')
    docfile.write('        if (elementDictionary != NULL)\n')
    docfile.write('        {\n')
    if evt.args:
        docfile.write('            ARCONTROLLER_DICTIONARY_ARG_t *arg = NULL;\n')
        docfile.write('            ARCONTROLLER_DICTIONARY_ELEMENT_t *dictElement = NULL;\n')
        docfile.write('            ARCONTROLLER_DICTIONARY_ELEMENT_t *dictTmp = NULL;\n')
        docfile.write('            HASH_ITER(hh, elementDictionary, dictElement, dictTmp)\n')
        docfile.write('            {\n')
        # print all args except the list flags
        for arg in [arg_tmp for arg_tmp in evt.args if not arg_tmp.name == _LIST_FLAG]:
            docfile.write('                HASH_FIND_STR (dictElement->arguments, '+defineNotification(feature, evt, arg)+', arg);\n')
            docfile.write('                if (arg != NULL)\n')
            docfile.write('                {\n')
            docfile.write('                    '+xmlToC(MODULE_ARCOMMANDS, feature, evt, arg)+' '+arg.name+' = arg->value.')
            if isinstance(arg.argType, ArEnum):
                docfile.write (ARCapitalize('i32'))
            elif isinstance(arg.argType, ArBitfield):
                docfile.write (ARCapitalize(ArArgType.TO_STRING[arg.argType.btfType]))
            else:
                docfile.write (ARCapitalize(ArArgType.TO_STRING[arg.argType]))
            docfile.write(';\n')
            docfile.write('                }\n')
        docfile.write('            }\n')
    else:
        docfile.write('\n')
    docfile.write('        }\n')
    docfile.write('        else\n')
    docfile.write('        {\n')
    docfile.write('            // list is empty\n')
    docfile.write('        }\n')
    docfile.write('    }\n')
    docfile.write('}\n')
    docfile.write('```\n\n')

def _write_event_list_java_code(docfile, feature, evt):
    docfile.write('```java\n')
    docfile.write('@Override\n')
    docfile.write('public void onCommandReceived (ARDeviceController deviceController, ARCONTROLLER_DICTIONARY_KEY_ENUM commandKey, ARControllerDictionary elementDictionary) {\n')
    docfile.write('    if (commandKey == ARCONTROLLER_DICTIONARY_KEY_ENUM.' + defineNotification(feature, evt) + '){\n')
    docfile.write('        if ((elementDictionary != null) && (elementDictionary.size() > 0)) {\n')
    if evt.args:
        docfile.write('            Iterator<ARControllerArgumentDictionary<Object>> itr = elementDictionary.values().iterator();\n')
        docfile.write('            while (itr.hasNext()) {\n')
        docfile.write('                ARControllerArgumentDictionary<Object> args = itr.next();\n')
        docfile.write('                if (args != null) {\n')
        for arg in [arg_tmp for arg_tmp in evt.args if not arg_tmp.name == _LIST_FLAG]:
            if isinstance(arg.argType, ArEnum):
                docfile.write('                    '+ xmlToJava(MODULE_ARCOMMANDS, feature, evt, arg) +' ' + arg.name + ' = ' + xmlToJava(MODULE_ARCOMMANDS, feature, evt, arg) + '.getFromValue((Integer)args.get(' + javaFeatureClassName(feature) +'.' + defineNotification(feature, evt, arg)+ '));\n')
            elif isinstance(arg.argType, ArBitfield):
                docfile.write('                    '+ xmlToJava(MODULE_ARCOMMANDS, feature, evt, arg) +' ' + arg.name + ' = (' + xmlToJava(MODULE_ARCOMMANDS, feature, evt, arg) + ')((Integer)args.get(' + javaFeatureClassName(feature) +'.' + defineNotification(feature, evt, arg)+ ')).intValue();\n')
            elif arg.argType == ArArgType.U8 or arg.argType == ArArgType.I8 or arg.argType == ArArgType.U16 or arg.argType == ArArgType.I16:
                docfile.write('                    '+ xmlToJava(MODULE_ARCOMMANDS, feature, evt, arg) +' ' + arg.name + ' = (' + xmlToJava(MODULE_ARCOMMANDS, feature, evt, arg) + ')((Integer)args.get(' + javaFeatureClassName(feature) +'.' + defineNotification(feature, evt, arg)+ ')).intValue();\n')
            elif arg.argType == ArArgType.FLOAT:
                docfile.write('                    '+ xmlToJava(MODULE_ARCOMMANDS, feature, evt, arg) +' ' + arg.name + ' = (' + xmlToJava(MODULE_ARCOMMANDS, feature, evt, arg) + ')((Double)args.get(' + javaFeatureClassName(feature) +'.' + defineNotification(feature, evt, arg)+ ')).doubleValue();\n')
            else:
                docfile.write('                    '+ xmlToJava(MODULE_ARCOMMANDS, feature, evt, arg) +' ' + arg.name + ' = (' + xmlToJava(MODULE_ARCOMMANDS, feature, evt, arg) + ')args.get(' + javaFeatureClassName(feature) +'.' + defineNotification(feature, evt, arg)+ ');\n')
        docfile.write('                }\n')
        docfile.write('            }\n')
    else:
        docfile.write('\n')
    docfile.write('        } else {\n')
    docfile.write('            // list is empty\n')
    docfile.write('        }\n')
    docfile.write('    }\n')
    docfile.write('}\n')
    docfile.write('```\n\n')

def _write_event_c_code(docfile, feature, evt):
    docfile.write('```c\n')
    docfile.write('void onCommandReceived (eARCONTROLLER_DICTIONARY_KEY commandKey, ARCONTROLLER_DICTIONARY_ELEMENT_t *elementDictionary, void *customData)\n')
    docfile.write('{\n')
    docfile.write('    if ((commandKey == '+defineNotification(feature, evt)+') && (elementDictionary != NULL))\n')
    docfile.write('    {\n')
    if evt.args:
        docfile.write('        ARCONTROLLER_DICTIONARY_ARG_t *arg = NULL;\n')
        docfile.write('        ARCONTROLLER_DICTIONARY_ELEMENT_t *element = NULL;\n')
        docfile.write('        HASH_FIND_STR (elementDictionary, ARCONTROLLER_DICTIONARY_SINGLE_KEY, element);\n')
        docfile.write('        if (element != NULL)\n')
        docfile.write('        {\n')
        for arg in evt.args:
            docfile.write('            HASH_FIND_STR (element->arguments, '+defineNotification(feature, evt, arg)+', arg);\n')
            docfile.write('            if (arg != NULL)\n')
            docfile.write('            {\n')
            docfile.write('                '+xmlToC(MODULE_ARCOMMANDS, feature, evt, arg)+' '+arg.name+' = arg->value.')
            if isinstance(arg.argType, ArEnum):
                docfile.write (ARCapitalize('i32'))
            elif isinstance(arg.argType, ArBitfield):
                docfile.write (ARCapitalize(ArArgType.TO_STRING[arg.argType.btfType]))
            else:
                docfile.write (ARCapitalize(ArArgType.TO_STRING[arg.argType]))
            docfile.write(';\n')
            docfile.write('            }\n')
        docfile.write('        }\n')
    else:
        docfile.write('\n')
    docfile.write('    }\n')
    docfile.write('}\n')
    docfile.write('```\n\n')


def _write_event_objc_code(docfile, feature, evt):
    docfile.write('```objective_c\n')
    docfile.write('void onCommandReceived (eARCONTROLLER_DICTIONARY_KEY commandKey, ARCONTROLLER_DICTIONARY_ELEMENT_t *elementDictionary, void *customData)\n')
    docfile.write('{\n')
    docfile.write('    if ((commandKey == '+defineNotification(feature, evt)+') && (elementDictionary != NULL))\n')
    docfile.write('    {\n')
    if evt.args:
        docfile.write('        ARCONTROLLER_DICTIONARY_ARG_t *arg = NULL;\n')
        docfile.write('        ARCONTROLLER_DICTIONARY_ELEMENT_t *element = NULL;\n')
        docfile.write('        HASH_FIND_STR (elementDictionary, ARCONTROLLER_DICTIONARY_SINGLE_KEY, element);\n')
        docfile.write('        if (element != NULL)\n')
        docfile.write('        {\n')
        for arg in evt.args:
            docfile.write('            HASH_FIND_STR (element->arguments, '+defineNotification(feature, evt, arg)+', arg);\n')
            docfile.write('            if (arg != NULL)\n')
            docfile.write('            {\n')
            docfile.write('                '+xmlToC(MODULE_ARCOMMANDS, feature, evt, arg)+' '+arg.name+' = arg->value.')
            if isinstance(arg.argType, ArEnum):
                docfile.write (ARCapitalize('i32'))
            elif isinstance(arg.argType, ArBitfield):
                docfile.write (ARCapitalize(ArArgType.TO_STRING[arg.argType.btfType]))
            else:
                docfile.write (ARCapitalize(ArArgType.TO_STRING[arg.argType]))
            docfile.write(';\n')
            docfile.write('            }\n')
        docfile.write('        }\n')
    else:
        docfile.write('\n')
    docfile.write('    }\n')
    docfile.write('}\n')
    docfile.write('```\n\n')

def _write_event_java_code(docfile, feature, evt):
    docfile.write('```java\n')
    docfile.write('@Override\n')
    docfile.write('public void onCommandReceived (ARDeviceController deviceController, ARCONTROLLER_DICTIONARY_KEY_ENUM commandKey, ARControllerDictionary elementDictionary) {\n')
    docfile.write('    if ((commandKey == ARCONTROLLER_DICTIONARY_KEY_ENUM.' + defineNotification(feature, evt) + ') && (elementDictionary != null)){\n')
    if evt.args:
        docfile.write('        ARControllerArgumentDictionary<Object> args = elementDictionary.get(ARControllerDictionary.ARCONTROLLER_DICTIONARY_SINGLE_KEY);\n')
        docfile.write('        if (args != null) {\n')
        for arg in evt.args:
            if isinstance(arg.argType, ArEnum):
                docfile.write('            '+ xmlToJava(MODULE_ARCOMMANDS, feature, evt, arg) +' ' + arg.name + ' = ' + xmlToJava(MODULE_ARCOMMANDS, feature, evt, arg) + '.getFromValue((Integer)args.get(' + javaFeatureClassName(feature) +'.' + defineNotification(feature, evt, arg) + '));\n')
            elif isinstance(arg.argType, ArBitfield):
                docfile.write('            '+ xmlToJava(MODULE_ARCOMMANDS, feature, evt, arg) +' ' + arg.name + ' = (' + xmlToJava(MODULE_ARCOMMANDS, feature, evt, arg) + ')((Integer)args.get(' + javaFeatureClassName(feature) +'.' + defineNotification(feature, evt, arg) + ')).intValue();\n')
            elif arg.argType == ArArgType.U8 or arg.argType == ArArgType.I8 or arg.argType == ArArgType.U16 or arg.argType == ArArgType.I16:
                docfile.write('            '+ xmlToJava(MODULE_ARCOMMANDS, feature, evt, arg) +' ' + arg.name + ' = (' + xmlToJava(MODULE_ARCOMMANDS, feature, evt, arg) + ')((Integer)args.get(' + javaFeatureClassName(feature) +'.' + defineNotification(feature, evt, arg)+ ')).intValue();\n')
            elif arg.argType == ArArgType.FLOAT:
                docfile.write('            '+ xmlToJava(MODULE_ARCOMMANDS, feature, evt, arg) +' ' + arg.name + ' = (' + xmlToJava(MODULE_ARCOMMANDS, feature, evt, arg) + ')((Double)args.get(' + javaFeatureClassName(feature) +'.' + defineNotification(feature, evt, arg)+ ')).doubleValue();\n')
            else:
                docfile.write('            '+ xmlToJava(MODULE_ARCOMMANDS, feature, evt, arg) +' ' + arg.name + ' = (' + xmlToJava(MODULE_ARCOMMANDS, feature, evt, arg) + ')args.get(' + javaFeatureClassName(feature) +'.' + defineNotification(feature, evt, arg)+ ');\n')
        docfile.write('        }\n')
    else:
        docfile.write('\n')
    docfile.write('    }\n')
    docfile.write('}\n')
    docfile.write('```\n\n')

def _write_event_triggered(ctx, docfile, triggered):
    if triggered:
        docfile.write ('\n\nTriggered ')
        for triggered_line in triggered.split('\n'):
            docfile.write(_replace_links(ctx, triggered_line) + '<br/>\n\n')

def _generate_doc(ctx, rootdir, features, args, product=None):
    if not os.path.exists(rootdir):
        os.makedirs(rootdir)

    has_events = dict()
    has_commands = dict()

    for feature in features:
        cmd_name = os.path.join(rootdir, _command_doc_file_name(feature))
        with open(cmd_name, 'w') as f:
            nb_cmd = _write_commands_doc(ctx, f, feature, args, product=product)
        has_commands[feature.name] = nb_cmd > 0
        if nb_cmd <= 0:
            os.unlink(cmd_name)

        evt_name = os.path.join(rootdir, _event_doc_file_name(feature))
        with open(evt_name, 'w') as f:
            nb_evt = _write_events_doc(ctx, f, feature, args, product=product)
        has_events[feature.name] = nb_evt > 0
        if nb_evt <= 0:
            os.unlink(evt_name)

    return (has_commands, has_events)

def _generate_toc(rootdir, cmds, evts, product):
    if product in DEVICES_IGNORED:
        return

    if not os.path.exists(rootdir):
        os.makedirs(rootdir)

    try:
        str_pr = DEVICE_TO_STRING[product]
    except KeyError:
        str_pr = product

    fname = os.path.join(rootdir, 'index.md')
    with open(fname, 'w') as f:
        f.write('---\n')
        f.write('title: libARController reference for %s\n' % str_pr)
        f.write('\n')
        f.write('language_tabs:\n')
        f.write('  - objective_c: Objective C\n')
        f.write('  - java: Java\n')
        f.write('  - c: C\n')
        f.write('\n')
        f.write('toc_footers:\n')
        f.write('  - <a href=\'http://forum.developer.parrot.com\'>Developer Forum</a>\n')
        f.write('  - <a href=\'https://github.com/Parrot-Developers/arsdk_manifest\'>See SDK sources</a>\n')
        f.write('  - <a href=\'http://github.com/tripit/slate\'>Documentation Powered by Slate</a>\n')
        f.write('\n')
        f.write('includes:\n')
        f.write('  - title\n')
        f.write('  - description\n')
        f.write('  - commands\n')
        for c in cmds:
            if cmds[c]:
                f.write('  - commands_' + c + '\n')
        f.write('  - events\n')
        for e in evts:
            if evts[e]:
                f.write('  - events_' + e + '\n')
        f.write('\n')
        f.write('search: true\n')
        f.write('---\n')


    fname = os.path.join(rootdir, '_title.md')
    with open(fname, 'w') as f:
        f.write('# %s reference\n' % str_pr)
        f.write('\n')
    fname = os.path.join(rootdir, '_commands.md')
    with open(fname, 'w') as f:
        f.write('## %s commands' % str_pr)
        f.write('\n')
    fname = os.path.join(rootdir, '_events.md')
    with open(fname, 'w') as f:
        f.write('## %s events' % str_pr)
        f.write('\n')

def _do_generate_files(ctx, outdir, args_as_arr=None):
    #parse args
    parser = argparse.ArgumentParser(description='Generate the documentation of messages.')
    parser.add_argument('-f', '--features', nargs='*', type=str, metavar='feature',
                    help='List of features to generate the documentation. All features will be generated if missing.')
    parser.add_argument('-w', '--warning', action='store_true',
                    help='Also generates warning logs if the feature does not contain new description or title too long or missing some tags.')
    parser.add_argument('-r', '--reference', action='store_true',
                    help='Generate reference documentation for all products.')

    # if args_as_arr is None, parse the args given in the command line
    args = parser.parse_args(args_as_arr)

    features = ctx.features if not args.features else [f for f in ctx.features if f.name in args.features]

    if args.reference:
        ref_dir = os.path.join(outdir, 'reference')
        products = ( x for x in DEVICE_TO_STRING if x not in (DEVICES_GLOBAL+DEVICES_IGNORED) )

        for p in products:
            name = DEVICE_TO_STRING[p].replace(' ', '_').lower()
            directory = os.path.join(ref_dir, name)
            (cmds, evts) = _generate_doc(ctx, directory, features, args, product=p)
            _generate_toc(directory, cmds, evts, p)
        directory = os.path.join(ref_dir, 'all')
        (cmds, evts) = _generate_doc(ctx, directory, features, args)
        _generate_toc(directory, cmds, evts, 'all products')

    else:
        doc_dir = os.path.join(outdir, 'documentation')
        _generate_doc(ctx, doc_dir, features, args)


def list_files(ctx, outdir, extra):
    for feature in ctx.features:
        print(os.path.join(outdir, _command_doc_file_name(feature)))
        print(os.path.join(outdir, _event_doc_file_name(feature)))

def generate_files(ctx, outdir, extra):
    args_as_arr = extra.split(' ')[1:]

    _do_generate_files(ctx, outdir, args_as_arr)

# if this script is called directly from the command line
if __name__ == '__main__':
    my_dir=os.path.abspath(os.path.dirname(__file__))
    message_path = os.path.join(my_dir, '..', '..', 'arsdk-xml', 'xml')

    ctx = ArParserCtx()
    _parse_features(ctx, message_path)

    _do_generate_files(ctx, my_dir)
