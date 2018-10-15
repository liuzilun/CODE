package logic

func PutMsgFromClient(msg string) error {

	pMsg := GetProductMsgInstance()
	pMsg.SetMsg(msg)
	pMsg.PublishMsg()
	return nil
}
