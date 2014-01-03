public class pair<T, U>
{
    public pair () {}

    public pair (T first, U second)
    {
        this.first = first;
        this.second = second;
    }

    public override bool Equals (object rhs_)
    {
        if (rhs_ == null)
            return false;
        pair<T, U> rhs = rhs_ as pair<T, U>;
        if (rhs == null)
            return false;

        return
            (first == null && rhs.first == null || first.Equals(rhs.first)) &&
            (second == null && rhs.second == null || second.Equals(rhs.second));
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
