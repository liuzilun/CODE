package msgserver

import (
	"testing"
	"wething/msgserver/msgserver"
)

func TestHandMsgFromMq(t *testing.T) {

	//

	//HandMsgFromMq()

	var pMsg *msgserver.MqMsqHand

	pMsg = &msgserver.MqMsqHand{}
	pMsg.Init()
	pMsg.MsgConsume()

}
