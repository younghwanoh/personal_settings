#!/usr/bin/env python

import json
import requests

def sendmessage(txt):
    webhook_url = 'https://hooks.slack.com/services/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' # nn
    slack_data = {'text': txt}
    response = requests.post(
        webhook_url, data=json.dumps(slack_data),
        headers={'Content-Type': 'application/json'}
    )
    if response.status_code != 200:
        raise ValueError(
            'Request to slack returned an error %s, the response is:\n%s'
            % (response.status_code, response.text)
        )

def testmessage(txt):
    webhook_url = 'https://hooks.slack.com/services/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' # my
    slack_data = {'text': txt}
    response = requests.post(
        webhook_url, data=json.dumps(slack_data),
        headers={'Content-Type': 'application/json'}
    )
    if response.status_code != 200:
        raise ValueError(
            'Request to slack returned an error %s, the response is:\n%s'
            % (response.status_code, response.text)
        )

if __name__ == "__main__":
    print("Sending test messages ...")
    testmessage("This is test of slackmessage :party-parrot:")
    print("Success !")
