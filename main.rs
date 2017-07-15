use std::net;

const BUFFSIZE : u32 = 1492;

enum ClnComEnum {
    FILEDATA,
    PKT,
}

#[repr(C)]
struct ClnCom {
    m_command : ClnComEnum,
    m_param : u32,
}

enum TrkCom {
    ADD,
    REMOVE,
    LIST,
}

#[repr(C)]
struct FileProps {
    m_size : u32,
    m_name : [char; 260],
}

fn p2p_tracker(first : &net::SocketAddrV4) {
    if let Ok(tracker) = net::UdpSocket::bind((net::Ipv4Addr::from(0), 0)) {
        if let Ok(_) = tracker.set_read_timeout(Some(std::time::Duration::new(1, 0))) {
            let mut connection_is_good = false;
            for i in 0..4 {
                tracker.send_to(&[0; 0], first);
                if let Ok((_, tracker_secure)) = tracker.recv_from(&mut [0; 0]) {
                    if tracker.local_addr().unwrap() != tracker_secure {
                        return
                    }
                    connection_is_good = true;
                    break
                }
            }
            if connection_is_good {
                tracker.set_read_timeout(None);
                let clients : std::vec::Vec<net::SocketAddrV4> = vec![*first];
                {
                    let mut i = 0;
                    loop {
                        let mut com : TrkCom;
                        if clients.len() - 1 < 32 {
                            i = 0;
                        }
                        if let Ok((size, net::SocketAddr::V4(client))) = tracker.recv_from(
                            std::slice::from_raw_parts_mut(
                                unsafe { &mut com as *mut _ as *mut u8 }, std::mem::size_of::<TrkCom>()
                            )
                        ) {
                            match com {
                                TrkCom::LIST => {
                                    tracker.send_to(
                                        std::slice::from_raw_parts(
                                            unsafe { &clients[i] as *const _ as *const u8 },
                                            std::cmp::min(32, clients.len()) * std::mem::size_of::<net::SocketAddrV4>()
                                        ),
                                        client
                                    );
                                }
                                TrkCom::ADD => {
                                    clients.push(client);
                                    tracker.send_to(
                                        std::slice::from_raw_parts(
                                            &com as *const _ as *mut u8, std::mem::size_of::<TrkCom>()
                                        ),
                                        client
                                    );
                                }
                                TrkCom::REMOVE => {
                                    if let Some(pos) = clients.iter().position(|&addr| addr == client) {
                                        clients.remove(pos);
                                        tracker.send_to(
                                            std::slice::from_raw_parts(
                                                &com as *const _ as *mut u8, std::mem::size_of::<TrkCom>()
                                            ),
                                            client
                                        );
                                    }
                                }
                            }
                        }
                        i = (i + 1) % clients.len();
                    }
                }
            }
        }
    }
}

fn main() {
    p2p_tracker();
}

