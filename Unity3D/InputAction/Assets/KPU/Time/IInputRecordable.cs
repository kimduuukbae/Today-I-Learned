using System.Collections.Generic;

namespace KPU.Time
{
    public interface IInputRecordable
    {
        IList<InputRecord> InputRecords { get; }
        void Record(InputRecord inputRecord);
    }
}