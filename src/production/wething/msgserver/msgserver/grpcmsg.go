package msgserver

import (
	"fmt"
	"time"
	pb "wething/grpcproto"

	"golang.org/x/net/context"
	"google.golang.org/grpc"
)

var (
	grpcclientcon *grpc.ClientConn
	grpcdbclient  pb.DbServerClient
	address       string
)

func init() {

	address = "127.0.0.1:50081"
	// Set up a connection to the server.
	grpcclientcon, err := grpc.Dial(address, grpc.WithInsecure())
	if err != nil {
		fmt.Printf("did not connect: %v", err)
	}
	grpcdbclient = pb.NewDbServerClient(grpcclientcon)

	fmt.Printf("NewDbServerClient success")

}
func RPCHandleMsgToDbserver(msg string) {

	ctx, cancel := context.WithTimeout(context.Background(), time.Second)
	defer cancel()
	r, err := grpcdbclient.HandleMsgFromMsgServer(ctx, &pb.MsgFromMsgServerRequest{MsgBody: msg})
	if err != nil {
		fmt.Printf("could not HandleMsgFromMsgServer: %v", err)
	}
	fmt.Printf("HandleMsgToDbserver: %s", r.Message)
}
