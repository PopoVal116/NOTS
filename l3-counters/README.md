# OAI L3 counters + Victoria Metrics

Лёгкий репозиторий: библиотека счётчиков и скрипт выгрузки в Victoria Metrics.
Интеграция в OAI — правки в исходниках openairinterface5g (см. таблицу ниже).

## Содержимое репозитория

| Путь | Назначение |
|------|------------|
| `counters/counters.h` | API библиотеки |
| `counters/counters.c` | Реализация |
| `push_oai_to_vm.py` | JSON → Victoria Metrics |

В дереве OAI библиотека лежит как:
`openairinterface5g/common/utils/counters/`

## Сборка в OAI

В корневом `CMakeLists.txt`:
- `add_library(COUNTERS ...)`
- линковка через `UTIL` (`target_link_libraries(UTIL ... COUNTERS)`)

Include в коде OAI: `#include "counters/counters.h"`

## Счётчики и места в коде OAI

### UE (`nr-uesoftmodem`)

| Имя счётчика | Файл | Событие |
|--------------|------|---------|
| `rrc_connected` | `openair2/RRC/NR_UE/rrc_UE.c` | переход в NR_RRC_CONNECTED |
| `nas_registration_accept` | `openair3/NAS/NR_UE/` (handler Registration Accept, напр. `nr_nas_msg_sim.c`) | приём Registration Accept |

Периодический dump:
- файл: `executables/nr-uesoftmodem.c`
- путь JSON: `/tmp/oai_ue_counters.json`

### gNB (`nr-softmodem`)

| Имя счётчика | Файл | Событие |
|--------------|------|---------|
| `gnb_rrc_setup_complete` | `openair2/RRC/NR/rrc_gNB.c` | лог Received RRCSetupComplete |
| `gnb_ng_setup_response` | `openair3/NGAP/ngap_gNB_handlers.c` | лог Received NGSetupResponse from AMF |

Периодический dump:
- файл: `executables/nr-softmodem.c`
- путь JSON: `/tmp/oai_gnb_counters.json`

## Victoria Metrics

```bash
docker run -d --name victoria-metrics -p 8428:8428 \
  victoriametrics/victoria-metrics:latest
python3 push_oai_to_vm.py```

# UI: http://127.0.0.1:8428/vmui/

###Стенд
OAI-CN5G (Docker) + gNB/UE на хосте, радио через ZeroMQ.
