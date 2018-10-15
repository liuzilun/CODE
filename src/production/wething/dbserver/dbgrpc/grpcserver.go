package protodbserver

import (
	"fmt"
	"net"

	"golang.org/x/net/context"
	"google.golang.org/grpc"
	"google.golang.org/grpc/reflection"

	"encoding/json"
	"wething/dbserver/db"
	pb "wething/grpcproto"
	"wething/model"
)

const (
	Address = "127.0.0.1:50081"
)

type dbService struct{}

func Init() error {

	lis, err := net.Listen("tcp", Address)
	if err != nil {
		fmt.Printf("failed to listen: %v", err)
		return err
	}
	s := grpc.NewServer()
	pb.RegisterDbServerServer(s, &dbService{})
	// Register reflection service on gRPC server.
	reflection.Register(s)

	if err := s.Serve(lis); err != nil {
		fmt.Printf("failed to serve: %v", err)
		return err
	}
	return nil
}

func (h dbService) HandleMsgFromMsgServer(ctx context.Context, in *pb.MsgFromMsgServerRequest) (*pb.MsgFromMsgServerReply, error) {
	resp := new(pb.MsgFromMsgServerReply)

	fmt.Printf("recive msg from client %v \n", in.MsgBody)

	//消息反序列化为消息结构
	msginfo := &model.WeThingDetail{}

	err := json.Unmarshal([]byte(in.MsgBody), msginfo)
	if err != nil {
		fmt.Printf("json marshal failed\n")
		//消息处理失败，需要处理
		return nil, err
	}

	weid, err := db.InsertMsg(msginfo)

	resp.Message = "Wething  " + in.MsgBody + string(weid)

	return resp, nil
}
