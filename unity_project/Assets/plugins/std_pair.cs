public struct pair<T, U>
{
    public pair (T first, U second)
    {
        this.first = first;
        this.second = second;
    }

    public override int GetHashCode ()
    {
        int retval = first == null ? 0 : first.GetHashCode();
        if (second != null)
            retval ^= second.GetHashCode();
        return retval;
    }

    public T first;
    public U second;
};
