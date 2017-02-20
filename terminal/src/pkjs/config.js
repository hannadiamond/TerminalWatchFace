module.exports = [
  {
    "type": "heading",
    "defaultValue": "App Configuration"
  },
  {
    "type": "text",
    "defaultValue": "Here is some introductory text."
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Colors"
      },
      {
        "type": "color",
        "messageKey": "BackgroundColor",
        "defaultValue": "0x000000",
        "label": "Background Color"
      },
      {
        "type": "color",
        "messageKey": "TextColor",
        "defaultValue": "0xFFFFFF",
        "label": "Text Color"
      }
    ]
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Computer Type"
      },
     {
				"type": "select",
				"messageKey": "ComputerType",
				"defaultValue": "0",
				"label": "Select Computer Type",
				"options": [
					{
						"label": "Linux",
						"value": "0" 
					},
					{
						"label": "Mac",
						"value": "1" 
					},
				]
      }
    ]
	},
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "More Settings"
      },
      {
				"type": "select",
				"messageKey": "DisconnectVibrate",
				"defaultValue": "0",
				"label": "Select Disconnect Vibration Type",
				"options": [
					{
						"label": "None",
						"value": "0" 
					},
					{
						"label": "Short vibration",
						"value": "1" 
					},
					{ 
						"label": "Long vibration",
						"value": "2" 
					},
          {
						"label": "Double vibration",
						"value": "3" 
					},
				]
			},
      {
				"type": "select",
				"messageKey": "Date",
				"defaultValue": "0",
				"label": "Select Date Type",
				"options": [
					{
						"label": "DDD MMM/DD",
						"value": "0" 
					},
					{
						"label": "MMMM DD, YYYY",
						"value": "1" 
					},
					{ 
						"label": "MM/DD/YY",
						"value": "2" 
					},
          {
						"label": "DDDD MM/DD/YY",
						"value": "3" 
					},
          {
						"label": "DDD MM/DD/YY",
						"value": "4" 
					}
				]
			},
    ]
  },
  {
    "type": "submit",
    "defaultValue": "Save Settings"
  }
];