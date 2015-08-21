#!/bin/sh

python ./Python/generateControllersSources.py -feature all || exit 1
python ../ARSDKBuildUtils/Utils/Python/ARSDK_PrebuildActions.py --lib libARController --root . || exit 1
